#include "crashrecovery.h"
#include "threads.h"
#include "TCPlib.h"
#include "define.h"
#include "define.h"
#include "messages.pb-c.h"
#include "protobufutils.h"
#include "server.h"
#include "chatstorage.h"
#include "boolean.h"
#include "clientlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUF_LEN 1024*1024

chatdb *chat_db;
int fifo_broadcast;
clientlist * clist;

void server(void)
{
	int *newfd;

	/* initialise chat storage */

	chat_db = CSinit();
	if(chat_db == NULL)
		exit(EXIT_FAILURE);

	// keyboard management theread
	pthread_t thread_keyboad;
	pthread_create(&thread_keyboad, NULL, read_commands, NULL);

	// crash recovery thereads
	pthread_t thread_send_fifo;
	pthread_create(&thread_send_fifo, NULL, CRserver_write, NULL);

	pthread_t thread_rcv_fifo;
	pthread_create(&thread_rcv_fifo, NULL, CRserver_read, NULL);

	/* create fifo for broadcast */
	char fifo_name_broadcast[strlen(FIFO_NAME_BROADCAST)+8];	// save some space for a PID

	sprintf(fifo_name_broadcast, "%s-%d", FIFO_NAME_BROADCAST, (int) getpid());

	if(mkfifo(fifo_name_broadcast, 0600) == -1)	// open for reading and writing so that it does not block
	{
		perror("create fifo broadcast");
		exit(EXIT_FAILURE);
	}
	fifo_broadcast = open(fifo_name_broadcast, O_RDWR /*| O_NONBLOCK*/);
	if(fifo_broadcast == -1)
	{
		perror("failed to open fifo for broadcast");
		exit(EXIT_FAILURE);
	}

	/* create thread for broadcast */
	pthread_t thread_chat_broadcast;
	pthread_create(&thread_chat_broadcast, NULL, broadcast_chat, NULL);

	/* initialise client list */
	clist = CLinit();


	while(1)
	{
		// accepts 
		newfd = malloc(sizeof(int));
		if(newfd == NULL)
		{
			perror("allocation for an incoming connection file descriptor");
			exit(EXIT_FAILURE);
		}
		*newfd = TCPaccept(TCPfd_global);
		if(*newfd < 0)
		{
			puts("TCPaccept() failed");
		}

		pthread_t thread_incoming_connection;
		pthread_create(&thread_incoming_connection, NULL, incoming_connection, newfd);
	}


	/* thread joins */
	pthread_join(thread_keyboad, NULL);	// wait for thread_keyboad termination
	pthread_join(thread_send_fifo, NULL);
	pthread_join(thread_rcv_fifo, NULL);
	puts("server function ended");
}


void * incoming_connection(void *arg)
{
	char *buf;
	int len_received, fd = *(int*)arg;
	ClientToServer *msg;
	boolean disc = false;
	boolean loggedin = false;

	while(!disc)
	{
		buf = NULL;
		len_received = PROTOrecv(fd, &buf);
		if(len_received < 0)
		{
			puts("PROTOrecv() failed");
			TCPclose(fd);
			free(buf);
			pthread_exit(NULL);
		}

		msg = client_to_server__unpack(NULL, len_received, (uint8_t*) buf);

		switch(msg->type)
		{
			case CLIENT_TO_SERVER__TYPE__LOGIN:
				puts("event login");
				loggedin = manage_login(fd, msg, loggedin);
				break;
			case CLIENT_TO_SERVER__TYPE__DISC:
				puts("event disc");
				disc = true;
				manage_disconnect(fd, loggedin);
				break;
			case CLIENT_TO_SERVER__TYPE__CHAT:
				puts("event chat");
				manage_chat(fd, msg, loggedin);
				break;
			case CLIENT_TO_SERVER__TYPE__QUERY:
				puts("event query");
				manage_query(fd, msg, loggedin);
				break;
			default:
				puts("event error");
		}

		//printf("%s\n", msg->str);
		#ifdef DEBUG
		CLprint(clist);
		#endif

		client_to_server__free_unpacked(msg, NULL);
		free(buf);
	}

	puts("bye");

	pthread_exit(NULL);
}


void * broadcast_chat(void *arg)
{
	char *buf;
	int len_received;
	ServerToBroadcast *msg;
	int fd_sender;
	char *chat;

	#ifdef DEBUG
	puts("broadcast thread started");
	#endif

	while(1)
	{
		buf = NULL;
		len_received = PROTOrecv(fifo_broadcast, &buf);
		if(len_received < 0)
		{
			perror("receive in broadcast fifo");
			//pthread_exit(NULL);
		}
		msg = server_to_broadcast__unpack(NULL, len_received, (uint8_t*) buf);
		free(buf);

		chat = msg->str;

		#ifdef DEBUG
		fd_sender = msg->fd;
		printf("broadcast fd=%d '%s'\n", fd_sender, chat);
		#endif

		// send message to every client
		char *buf;
		ServerToClient msgStC = SERVER_TO_CLIENT__INIT;
		msgStC.n_str = 1;
		msgStC.str = &chat;

		buf = malloc(server_to_client__get_packed_size(&msgStC));
		if(buf == NULL)
		{
			perror("malloc in broadcast pack");
			exit(EXIT_FAILURE);
		}

		server_to_client__pack(&msgStC, (uint8_t*) buf);
		CLbroadcast(clist, buf, server_to_client__get_packed_size(&msgStC));

		free(buf);
		server_to_broadcast__free_unpacked(msg, NULL);
	}
}


int manage_login(int fd, ClientToServer *msg, int loggedin)
{
	char *buf;
	ServerToClient msgStC = SERVER_TO_CLIENT__INIT;
	char *username = msg->str;

	msgStC.has_code = true;
	if(!loggedin)
	{
		// verify if there is no user with this username yet
		if(CLadd(&clist, fd, username)) // username does not exist
		{
			msgStC.code = SERVER_TO_CLIENT__CODE__OK;
			loggedin = true;
		}
		else	// username already exists. send NOK
			msgStC.code = SERVER_TO_CLIENT__CODE__NOK;
	}
	else
	{
		// send NOK
		msgStC.code = SERVER_TO_CLIENT__CODE__NOK;
	}

	buf = malloc(server_to_client__get_packed_size(&msgStC));
	if(buf == NULL)
	{
		perror("malloc in login processing");
		exit(EXIT_FAILURE);
	}

	server_to_client__pack(&msgStC, (uint8_t*) buf);
	if(PROTOsend(fd, (char*) buf, server_to_client__get_packed_size(&msgStC)) != 0)
	{
		puts("Failed to reply to login message");
	}
	free(buf);

	return loggedin;
}


void manage_disconnect(int fd, int loggedin)
{
	if(loggedin)
	{
		// remove client from list
		clist = CLremove(clist, fd);
	}

	TCPclose(fd);
}

void manage_query(int fd, ClientToServer *msg, int loggedin)
{
	if(!loggedin)
		return;

	// send messages back to client
	char *buf;
	ServerToClient msgStC = SERVER_TO_CLIENT__INIT;
	char **messages;

	if(!msg->has_id_min || !msg->has_id_max)
		return;
	messages = CSquery(chat_db, msg->id_min, msg->id_max);
	if(messages == NULL)
	{
		puts("error while retrieving messages");
		return;
	}

	int nr_messages;
	for(nr_messages=0; messages[nr_messages] != NULL; nr_messages++)
		;
	msgStC.str = messages;
	msgStC.n_str = nr_messages;

	buf = malloc(server_to_client__get_packed_size(&msgStC));
	if(buf == NULL)
	{
		perror("malloc in message query processing");
		exit(EXIT_FAILURE);
	}

	server_to_client__pack(&msgStC, (uint8_t*) buf);
	if(PROTOsend(fd, (char*) buf, server_to_client__get_packed_size(&msgStC)) != 0)
	{
		puts("failed to reply to query message");
	}

	// save to the log
	free(buf);
}


void manage_chat(int fd, ClientToServer *msg, int loggedin)
{
	if(!loggedin)
		return;

	char *buf;
	char *chat = msg->str;
	ServerToBroadcast msgStB = SERVER_TO_BROADCAST__INIT;

	#ifdef DEBUG
	puts(chat);
	#endif

	// send to the broadcast task
	msgStB.fd = fd;
	msgStB.str = chat;
	buf = malloc(server_to_broadcast__get_packed_size(&msgStB));
	if(buf == NULL)
	{
		perror("malloc in manage_chat()");
		exit(EXIT_FAILURE);
	}

	server_to_broadcast__pack(&msgStB, (uint8_t*) buf);
	if(PROTOsend(fifo_broadcast, (char*) buf, server_to_broadcast__get_packed_size(&msgStB)) != 0)
	{
		perror("Failed to send to broadcast task");
	}
	free(buf);

	// store chat
	if(CSstore(chat_db, chat) != 0)
	{
		perror("failed to store message");
		exit(EXIT_FAILURE);
	}

	// save to the log
}

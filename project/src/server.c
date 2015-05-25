#include "crashrecovery.h"
#include "TCPlib.h"
#include "define.h"
#include "define.h"
#include "messages.pb-c.h"
#include "protobufutils.h"
#include "server.h"
#include "chatstorage.h"
#include "boolean.h"
#include "clientlist.h"
#include "logging.h"
#include "controllerfifos.h"
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

#define LOG_CHAT "CHAT"
#define LOG_DISC "DISCONNECT"
#define LOG_LOGIN "LOGIN"
#define LOG_QUERY "QUERY"
#define LOG_START "START"
#define LOG_STOP "STOP"

chatdb *chat_db;
int fifo_broadcast;
clientlist * clist;
unsigned log_event_nr = 1;

void server(void)
{
	int *newfd;

	/* log server startup */
	LOGadd(LOGfd_global, log_event_nr++, LOG_START);

	/* initialise chat storage */
	chat_db = CSinit();
	if(chat_db == NULL)
		exit(EXIT_FAILURE);

	/* initialise client list */
	clist = CLinit();

	// crash recovery thereads
	pthread_t thread_send_fifo;
	pthread_create(&thread_send_fifo, NULL, CRserver_write, NULL);

	pthread_t thread_rcv_fifo;
	pthread_create(&thread_rcv_fifo, NULL, CRserver_read, NULL);

	pthread_t thread_killzombies;
	pthread_create(&thread_killzombies, NULL, CRkillzombies, NULL);

	/* create thread for broadcast */
	pthread_t thread_chat_broadcast;
	pthread_create(&thread_chat_broadcast, NULL, broadcast_chat, NULL);

	/* create thread for keyboard */
	pthread_t thread_keybd;
	pthread_create(&thread_keybd, NULL, server_keyboard, NULL);


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
			perror("TCPaccept() failed");
		}

		pthread_t thread_incoming_connection;
		pthread_create(&thread_incoming_connection, NULL, incoming_connection, newfd);
	}

	/* thread joins */
	pthread_join(thread_send_fifo, NULL);
	pthread_join(thread_rcv_fifo, NULL);
	pthread_join(thread_killzombies, NULL);
	pthread_join(thread_chat_broadcast, NULL);
	pthread_join(thread_keybd, NULL);
	#ifdef DEBUG
	puts("server function ended");
	#endif
}


void * incoming_connection(void *arg)
{
	char *buf, *username;
	int len_received, fd = *(int*)arg;
	ClientToServer *msg;
	boolean disc = false;
	boolean loggedin = false;

	free(arg);

	while(!disc)
	{
		buf = NULL;
		len_received = PROTOrecv(fd, &buf);
		if(len_received < 0)
		{
			#ifdef DEBUG
			perror("PROTOrecv() failed");
			#endif
			TCPclose(fd);
			free(buf);
			pthread_exit(NULL);
		}

		msg = client_to_server__unpack(NULL, len_received, (uint8_t*) buf);

		switch(msg->type)
		{
			case CLIENT_TO_SERVER__TYPE__LOGIN:
				#ifdef DEBUG
				puts("event login");
				#endif
				loggedin = manage_login(fd, msg, loggedin, &username);
				break;
			case CLIENT_TO_SERVER__TYPE__DISC:
				#ifdef DEBUG
				puts("event disc");
				#endif
				disc = true;
				manage_disconnect(fd, loggedin, username);
				break;
			case CLIENT_TO_SERVER__TYPE__CHAT:
				#ifdef DEBUG
				puts("event chat");
				#endif
				manage_chat(fd, msg, loggedin, username);
				break;
			case CLIENT_TO_SERVER__TYPE__QUERY:
				#ifdef DEBUG
				puts("event query");
				#endif
				manage_query(fd, msg, loggedin, username);
				break;
			default:
				puts("event error");
		}

		//printf("%s\n", msg->str);
		#ifdef DEBUG
		pthread_mutex_lock(&mutex_clist);	// lock
		CLprint(clist);
		pthread_mutex_unlock(&mutex_clist);	// unlock
		#endif

		client_to_server__free_unpacked(msg, NULL);
		free(buf);
	}

	if(loggedin == true)
		free(username);

	#ifdef DEBUG
	puts("bye");
	#endif

	pthread_exit(NULL);
}


void * broadcast_chat(void *arg)
{
	char *buf;
	int len_received;
	ServerToBroadcast *msg;
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
		int fd_sender;
		fd_sender = msg->fd;
		printf("broadcast fd=%d '%s'\n", fd_sender, chat);
		#endif

		// send message to every client
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
		pthread_mutex_lock(&mutex_clist);	// lock
		CLbroadcast(clist, buf, server_to_client__get_packed_size(&msgStC));
		pthread_mutex_unlock(&mutex_clist);	// unlock

		free(buf);
		server_to_broadcast__free_unpacked(msg, NULL);
	}
}


void * server_keyboard(void *var)
{
	int len_received;
	char *buf;
	ControllerToServer *msg;

	#ifdef DEBUG
	puts("starting server_keyboard()");
	#endif

	while(1)
	{
		buf = NULL;
		len_received = PROTOrecv(fifo_keybd_server, &buf);
		if(len_received < 0)
		{
			free(buf);
			continue;
		}

		msg = controller_to_server__unpack(NULL, len_received, (uint8_t*) buf);

		switch(msg->type)
		{
			case CONTROLLER_TO_SERVER__TYPE__LOG:
				puts("Received LOG command. Printing log...");
				
				char aux[100];
				int log_read_len;

				pthread_mutex_lock(&mutex_log);	// lock
				lseek(LOGfd_global, 0, SEEK_SET);
				do
				{
					log_read_len = read(LOGfd_global, aux, 100);
					write(STDOUT_FILENO, aux, log_read_len);
				}
				while(log_read_len != 0);
				pthread_mutex_unlock(&mutex_log);	// unlock
				break;
			case CONTROLLER_TO_SERVER__TYPE__QUIT:
				// clean memory and quit
				puts("Received QUIT command. Server is closing...");
				pthread_mutex_lock(&mutex_log);	// lock
				LOGadd(LOGfd_global, log_event_nr++, LOG_STOP);
				pthread_mutex_unlock(&mutex_log);	// unlock

				CSdestroy(chat_db);
				CLdestroy(clist);

				pthread_mutex_destroy(&mutex_chatdb);
				pthread_mutex_destroy(&mutex_clist);
				pthread_mutex_destroy(&mutex_log);
				exit(EXIT_SUCCESS);
				break;
			default:
				break;
		}

		free(buf);
		free(msg);
	}
}


/* 'loggedin' is the current login status.
 * 'username' is modified only if loggin succeeds */
int manage_login(int fd, ClientToServer *msg, int loggedin, char **username)
{
	char *buf;
	ServerToClient msgStC = SERVER_TO_CLIENT__INIT;

	msgStC.has_code = true;
	if(!loggedin)
	{
		*username = strdup(msg->str);
		// verify if there is no user with this username yet
		pthread_mutex_lock(&mutex_clist);	// lock
		if(CLadd(&clist, fd, *username)) // username does not exist
		{
			msgStC.code = SERVER_TO_CLIENT__CODE__OK;
			loggedin = true;
		}
		else	// username already exists. send NOK
			msgStC.code = SERVER_TO_CLIENT__CODE__NOK;
		pthread_mutex_unlock(&mutex_clist);	// unlock
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
		perror("Failed to reply to login message");
	}
	free(buf);

	// add event to the log
	char log_line[strlen(LOG_LOGIN) + strlen(msg->str) + 10];	// some spare bytes
	sprintf(log_line, "%s %s", LOG_LOGIN, msg->str);
	pthread_mutex_lock(&mutex_log);	// lock
	LOGadd(LOGfd_global, log_event_nr++, log_line);
	pthread_mutex_unlock(&mutex_log);	// unlock

	return loggedin;
}


void manage_disconnect(int fd, int loggedin, char *username)
{
	if(!loggedin)
		return;

	// remove client from list
	pthread_mutex_lock(&mutex_clist);	// lock
	clist = CLremove(clist, fd);
	pthread_mutex_unlock(&mutex_clist);	// unlock

	TCPclose(fd);

	// add event to the log
	char log_line[strlen(LOG_DISC) + strlen(username) + 10];	// spare bytes for numbers spaces, etc
	sprintf(log_line, "%s %s", LOG_DISC, username);
	pthread_mutex_lock(&mutex_log);	// lock
	LOGadd(LOGfd_global, log_event_nr++, log_line);
	pthread_mutex_unlock(&mutex_log);	// unlock
}


void manage_query(int fd, ClientToServer *msg, int loggedin, char *username)
{
	if(!loggedin)
		return;

	// send messages back to client
	char *buf;
	ServerToClient msgStC = SERVER_TO_CLIENT__INIT;
	char **messages;

	if(!msg->has_id_min || !msg->has_id_max)
		return;
	pthread_mutex_lock(&mutex_chatdb);	// lock
	messages = CSquery(chat_db, msg->id_min, msg->id_max);
	pthread_mutex_unlock(&mutex_chatdb);	// unlock
	if(messages == NULL)
	{
		perror("error while retrieving messages");
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
		perror("failed to reply to query message");
	}

	free(buf);

	// save to the log
	char log_line[strlen(LOG_QUERY) + strlen(username) + 32];	// spare bytes for numbers spaces, etc
	sprintf(log_line, "%s %s %lu %lu", LOG_QUERY, username,(unsigned long) msg->id_min, (unsigned long) msg->id_max);
	pthread_mutex_lock(&mutex_log);	// lock
	LOGadd(LOGfd_global, log_event_nr++, log_line);
	pthread_mutex_unlock(&mutex_log);	// unlock
}


void manage_chat(int fd, ClientToServer *msg, int loggedin, char *username)
{
	if(!loggedin)
		return;

	char *buf;
	char *chat = msg->str;
	char *chat_to_store;
	ServerToBroadcast msgStB = SERVER_TO_BROADCAST__INIT;

	#ifdef DEBUG
	puts(chat);
	#endif

	chat_to_store = malloc(sizeof(char)*(strlen(username) + strlen(": ") + strlen(chat) + 1));	// +1 is for \0
	if(chat_to_store == NULL)
	{
		perror("malloc in manage_chat()");
		exit(EXIT_FAILURE);
	}
	sprintf(chat_to_store, "%s: %s", username, chat);

	// send to the broadcast task
	//msgStB.fd = fd;
	msgStB.str = chat_to_store;
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
	pthread_mutex_lock(&mutex_chatdb);	// lock
	if(CSstore(chat_db, chat_to_store) != 0)
	{
		perror("failed to store message");
		exit(EXIT_FAILURE);
	}
	pthread_mutex_unlock(&mutex_chatdb);	// unlock
	free(chat_to_store);

	// add event to the log
	char log_line[strlen(LOG_CHAT) + strlen(username) + 10];	// some spare bytes
	sprintf(log_line, "%s %s", LOG_CHAT, username);
	pthread_mutex_lock(&mutex_log);	// lock
	LOGadd(LOGfd_global, log_event_nr++, log_line);
	pthread_mutex_unlock(&mutex_log);	// unlock
}


void setup_fifo_broadcast()
{
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
}

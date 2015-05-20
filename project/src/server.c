#include "crashrecovery.h"
#include "threads.h"
#include "TCPlib.h"
#include "define.h"
#include "define.h"
#include "messages.pb-c.h"
#include "protobufutils.h"
#include "server.h"
#include "chatstorage.h"
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
	sleep(10000);
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
		len_received = PROTOrecv(fd, &buf);
		if(len_received < 0)
		{
			puts("PROTOrecv() failed");
			TCPclose(fd);
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
				void manage_disconnect(int fd, int loggedin);
				break;
			case CLIENT_TO_SERVER__TYPE__CHAT:
				puts("event chat");
				if(loggedin)
				{
					// send chat to everyone
				}
				break;
			case CLIENT_TO_SERVER__TYPE__QUERY:
				puts("event query");
				manage_query(fd, msg, loggedin);
				break;
			default:
				puts("event error");
		}

		//printf("%s\n", msg->str);

		client_to_server__free_unpacked(msg, NULL);
		free(buf);
	}

	puts("bye");

	pthread_exit(NULL);
}


int manage_login(int fd, ClientToServer *msg, int loggedin)
{
	char *buf;
	ServerToClient msgStC = SERVER_TO_CLIENT__INIT;
	char *username = msg->str;

	msgStC.has_code = true;
	if(!loggedin)
	{
		loggedin = true;
		// verify if there is no user with this username yet
		// if(username does not exist)
		msgStC.code = SERVER_TO_CLIENT__CODE__OK;
		// else
		//		msgStC.code = SERVER_TO_CLIENT__CODE__NOK;
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

	if(loggedin)
	{
		// save name, fd, etc
	}
	free(buf);

	return loggedin;
}


void manage_disconnect(int fd, int loggedin)
{
	if(loggedin)
	{
		// remove name, fd, etc
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
		puts("Failed to reply to query message");
	}

	// save to the log
	free(buf);
}

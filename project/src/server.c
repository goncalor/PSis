#include "crashrecovery.h"
#include "threads.h"
#include "TCPlib.h"
#include "define.h"
#include "define.h"
#include "messages.pb-c.h"
#include "protobufutils.h"
#include "server.h"
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

void server(void)
{
	int *newfd;

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

	len_received = PROTOrecv(fd, &buf);
	if(len_received < 0)
	{
		puts("PROTOrecv() failed");
		pthread_exit(NULL);
	}

	msg = client_to_server__unpack(NULL, len_received, (uint8_t*) buf);

	switch(msg->type)
	{
		case CLIENT_TO_SERVER__TYPE__LOGIN:
			puts("event login");
			break;
		case CLIENT_TO_SERVER__TYPE__DISC:
			puts("event disc");
			break;
		case CLIENT_TO_SERVER__TYPE__CHAT:
			puts("event chat");
			break;
		case CLIENT_TO_SERVER__TYPE__QUERY:
			puts("event query");
			break;
		default:
			puts("event error");
	}

	printf("%s\n", msg->str);

	client_to_server__free_unpacked(msg, NULL);
	free(buf);

	pthread_exit(NULL);
}

#include "utils.h"
#include "inetutils.h"
#include "define.h"
#include "TCPlib.h"
#include "chatstorage.h"
#include "threads.h"
#include "fifo.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define LISTEN_MAX 16

void server(fifo_msg *msg_fifo);
void relauncher(fifo_msg *msg_fifo);

int main(int argc, char **argv)
{
	unsigned short port=3000;
	//char username[];

	//version("1.0");

/*-------- check arguments --------*/

	int opt;

	while((opt=getopt(argc, argv, "p:"))!=-1) /*getopt() returns one arg at a time. sets optarg*/
	{
		switch(opt)
		{
			case 'p': port = atoi(optarg); break;
			case '?': usage(argv[0]); exit(EXIT_SUCCESS);
		}
	}

	#ifdef DEBUG
	printf("port: %hu\n", port);
	#endif

	putchar('\n');
	listcommands();
	putchar('\n');

/*-------- END check arguments --------*/

	/* initialise chat storage */
	chatdb *chatdb;

	chatdb = CSinit();
	if(chatdb == NULL)
		exit(EXIT_FAILURE);

	/* create a socket and listen */
	int TCPfd;

	TCPfd = TCPcreate(INADDR_ANY, port);
	if(TCPfd < 0)
	{
		puts("failed to create listening socket");
		exit(EXIT_FAILURE);
	}

	if(listen(TCPfd, LISTEN_MAX) == -1)
	{
		perror("failed to listen()");
		exit(EXIT_FAILURE);
	}

	/* create FIFOs */
	char fifo_name_server[strlen(FIFO_NAME_SERVER)+8];
	char fifo_name_relauncher[strlen(FIFO_NAME_RELAUNCHER)+8];

	sprintf(fifo_name_server, "%s-%d", FIFO_NAME_SERVER, (int) getpid());
	sprintf(fifo_name_relauncher, "%s-%d", FIFO_NAME_RELAUNCHER, (int) getpid());

	if(mkfifo(fifo_name_server, 0600) == -1)	// open for reading and writing so that it does not block
	{
		perror("create fifo server");
		exit(EXIT_FAILURE);
	}

	if(mkfifo(fifo_name_relauncher, 0600) == -1)	// open for reading and writing so that it does not block
	{
		perror("create fifo relauncher");
		exit(EXIT_FAILURE);
	}

	fifo_msg server_msg, relauncher_msg;

	strcpy(server_msg.fifo_read, fifo_name_relauncher);
	strcpy(server_msg.fifo_write, fifo_name_server);
	server_msg.func = relauncher;

	strcpy(relauncher_msg.fifo_read, fifo_name_server);
	strcpy(relauncher_msg.fifo_write, fifo_name_relauncher);
	relauncher_msg.func = server;

	/* create server and relauncher */
	if(fork() == 0)
	{
		server(&server_msg);
	}
	else
	{
		relauncher(&relauncher_msg);
	}

	exit(EXIT_SUCCESS);
}




void * rcv_fifo(void * msg)
{
	int fd;
	char buf[FIFO_BUF];
	ssize_t retval;
	fifo_msg *msg_fifo = (fifo_msg*) msg;

	fd = open(msg_fifo->fifo_read, O_RDONLY);
	if(fd == -1)
	{
		perror("fifo open receive");
		exit(EXIT_FAILURE);
	}

	fifo_msg fifo_child;

	if(msg_fifo->func == server)
		fifo_child.func = relauncher;
	else
		fifo_child.func = server;

	strcpy(fifo_child.fifo_read, msg_fifo->fifo_write);
	strcpy(fifo_child.fifo_write, msg_fifo->fifo_read);
	printf("send %lu receive %lu\n", fifo_child.func, msg_fifo->func);

	while(1)
	{
		retval = read(fd, &buf, sizeof(buf));
		if(retval == -1)
		{
			perror("read from server fifo");
		}
		else if(retval == 0)
		{
			if(fork() == 0)
			{
				printf("%lu\n",fifo_child.func);
				msg_fifo->func(&fifo_child);
			}
		}

		sleep(TIMEOUT);
	}
}


void * send_fifo(void * fifo_name)
{
	int fd;
	char buf;

	puts(fifo_name);
	fd = open((char*) fifo_name, O_WRONLY);
	if(fd == -1)
	{
		perror("fifo open send");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		if(write(fd, &buf, sizeof(buf)) == -1)
			perror("write to server fifo");
		sleep(TIMEOUT/2);
	}
}


void server(fifo_msg *msg_fifo)
{
	// create thread to manage keyboard

	printf("server %s %s %lu\n", msg_fifo->fifo_write, msg_fifo->fifo_read, msg_fifo->func);

	pthread_t thread_keyboad;
	pthread_create(&thread_keyboad, NULL, read_commands, NULL);

	pthread_t thread_send_fifo;
	pthread_create(&thread_send_fifo, NULL, send_fifo, msg_fifo->fifo_write);

	pthread_t thread_rcv_fifo;
	pthread_create(&thread_rcv_fifo, NULL, rcv_fifo, msg_fifo);

	/* thread joins */
	pthread_join(thread_keyboad, NULL);	// wait for thread_keyboad termination
	pthread_join(thread_send_fifo, NULL);
	pthread_join(thread_rcv_fifo, NULL);
}


void relauncher(fifo_msg *msg_fifo)
{
	printf("relauncher %s %s %lu\n", msg_fifo->fifo_write, msg_fifo->fifo_read, msg_fifo->func);

	pthread_t thread_send_fifo;
	pthread_create(&thread_send_fifo, NULL, send_fifo, msg_fifo->fifo_write);

	pthread_t thread_rcv_fifo;
	pthread_create(&thread_rcv_fifo, NULL, rcv_fifo, msg_fifo);

	/* thread joins */
	pthread_join(thread_send_fifo, NULL);
	pthread_join(thread_rcv_fifo, NULL);
}






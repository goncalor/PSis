#include "utils.h"
#include "inetutils.h"
#include "define.h"
#include "TCPlib.h"
#include "chatstorage.h"
#include "threads.h"
#include "crashrecovery.h"
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
#include <signal.h>


#define LISTEN_MAX 16

void server();
void relauncher();

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

	/* setup crash recovery */
	CRsetup();

	/* create server and relauncher */
	if(fork() == 0)
	{
		server();
	}
	else
	{
		relauncher();
	}

	exit(EXIT_SUCCESS);
}





void server(void)
{
	// keyboard management theread
	pthread_t thread_keyboad;
	pthread_create(&thread_keyboad, NULL, read_commands, NULL);

	// crash recovery thereads
	pthread_t thread_send_fifo;
	pthread_create(&thread_send_fifo, NULL, CRserver_write, NULL);

	pthread_t thread_rcv_fifo;
	pthread_create(&thread_rcv_fifo, NULL, CRserver_read, NULL);

	/* thread joins */
	sleep(10000);
	pthread_join(thread_keyboad, NULL);	// wait for thread_keyboad termination
	pthread_join(thread_send_fifo, NULL);
	pthread_join(thread_rcv_fifo, NULL);
	puts("server function ended");
}


void relauncher(void)
{

	// crash recovery thereads
	pthread_t thread_send_fifo;
	pthread_create(&thread_send_fifo, NULL, CRrelauncher_write, NULL);

	pthread_t thread_rcv_fifo;
	pthread_create(&thread_rcv_fifo, NULL, CRrelauncher_read, NULL);

	/* thread joins */
	sleep(10000);
	pthread_join(thread_send_fifo, NULL);
	pthread_join(thread_rcv_fifo, NULL);
	puts("relauncher function ended");
}



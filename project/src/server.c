#include "utils.h"
#include "inetutils.h"
#include "define.h"
#include "TCPlib.h"
#include "chatstorage.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>

#define BUF_LEN 1024
#define COMM_LEN 40
#define LISTEN_MAX 16

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


	exit(EXIT_SUCCESS);
}

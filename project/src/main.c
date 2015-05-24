#include "utils.h"
#include "inetutils.h"
#include "define.h"
#include "TCPlib.h"
#include "chatstorage.h"
#include "crashrecovery.h"
#include "server.h"
#include "relauncher.h"
#include "controllerfifos.h"
#include "logging.h"
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
#define LOG_NAME	"logs/server"
#define LOG_NAME_EXT	"log"


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

	/* create a socket and listen */
	TCPfd_global = TCPcreate(INADDR_ANY, port);
	if(TCPfd_global < 0)
	{
		puts("failed to create listening socket");
		exit(EXIT_FAILURE);
	}

	if(listen(TCPfd_global, LISTEN_MAX) == -1)
	{
		perror("failed to listen()");
		exit(EXIT_FAILURE);
	}

	/* setup crash recovery */
	CRsetup();

	/* setup server controller */
	setup_controllers();

	/* setup server log file */
	LOGfd_global = LOGcreate(LOG_NAME, LOG_NAME_EXT);
	if(LOGfd_global == -1)
	{
		perror("failded to create log");
		exit(EXIT_FAILURE);
	}

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


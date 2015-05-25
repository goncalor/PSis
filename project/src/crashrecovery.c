#include "crashrecovery.h"
#include "server.h"
#include "relauncher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>


void CRsetup()
{
	/* create FIFOs */
	char fifo_name_server[strlen(FIFO_NAME_SERVER)+8];	// save some space for a PID
	char fifo_name_relauncher[strlen(FIFO_NAME_RELAUNCHER)+8];

	sprintf(fifo_name_server, "%s-%d", FIFO_NAME_SERVER, (int) getpid());
	sprintf(fifo_name_relauncher, "%s-%d", FIFO_NAME_RELAUNCHER, (int) getpid());

	#ifdef DEBUG
	printf("fifo_name_server %s\n", fifo_name_server); 
	printf("fifo_name_relauncher %s\n", fifo_name_relauncher); 
	#endif

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

	/* create file descriptors for both ends */
	
	fifo_server = open(fifo_name_server, O_RDWR | O_NONBLOCK);
	if(fifo_server == -1)
	{
		perror("Open fifo from server");	
		exit(EXIT_FAILURE);
	}

	fifo_relauncher = open(fifo_name_relauncher, O_RDWR | O_NONBLOCK);
	if(fifo_relauncher == -1)
	{
		perror("Open fifo from relauncher");	
		exit(EXIT_FAILURE);
	}

	signal(SIGPIPE, SIG_IGN);	// needed so that there is no program termination when write() tries to write to a broken pipe
	signal(SIGINT, SIG_IGN);	// ignore ctrl-c
}


void * CRserver_read(void *var)
{
	ssize_t retval;
	char buf[FIFO_BUF];

	while(1)
	{
		sleep(TIMEOUT);
		retval = read(fifo_server, &buf, FIFO_BUF);
		#ifdef DEBUG
		printf("CRserver_read %d\n", (int) retval);
		#endif
		if(retval == -1)
		{
			if(fork() == 0)
			{
				#ifdef DEBUG
				puts("relaunching relauncher...");
				#endif
				relauncher();
			}
		}
	}
}

void * CRrelauncher_read(void *var)
{
	ssize_t retval;
	char buf[FIFO_BUF];

	while(1)
	{
		sleep(TIMEOUT);
		retval = read(fifo_relauncher, &buf, FIFO_BUF);
		#ifdef DEBUG
		printf("CRrelauncher_read %d\n", (int) retval);
		#endif
		if(retval == -1)
		{
			if(fork() == 0)
			{
				#ifdef DEBUG
				puts("relaunching server...");
				#endif
				server();
			}
		}
	}
}

void * CRserver_write(void *var)
{
	#ifdef DEBUG
	ssize_t retval;
	#endif
	char buf=0;	// this can be anything. we just need to write something

	while(1)
	{
		#ifdef DEBUG
		retval = write(fifo_relauncher, &buf, sizeof(char));
		printf("CRserver_write %d\n", (int) retval);
		#else
		write(fifo_relauncher, &buf, sizeof(char));
		#endif
		sleep(TIMEOUT/2);
	}
}

void * CRrelauncher_write(void *var)
{
	#ifdef DEBUG
	ssize_t retval;
	#endif
	char buf=0;	// this can be anything. we just need to write something

	while(1)
	{
		#ifdef DEBUG
		retval = write(fifo_server, &buf, sizeof(char));
		printf("CRrelauncher_write %d\n", (int) retval);
		#else
		write(fifo_server, &buf, sizeof(char));
		#endif
		sleep(TIMEOUT/2);
	}
}


void *CRkillzombies(void *args)
{
	while(1)
	{
		sleep(TIMEOUT);
		wait(NULL);
	}
}

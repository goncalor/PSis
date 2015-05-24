#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "controllerfifos.h"
#include "protobufutils.h"
#include "messages.pb-c.h"
#include "define.h"


void setup_server_controller()
{
	char fifo_name_server[strlen(FIFO_KEYBD_NAME) + strlen("server-") + 8]; // save some space for a PID
	char fifo_name_relauncher[strlen(FIFO_KEYBD_NAME) + strlen("relauncher-") + 8];

	sprintf(fifo_name_server, "%s-server-%d", FIFO_KEYBD_NAME, (int) getpid());
	sprintf(fifo_name_relauncher, "%s-relauncher-%d", FIFO_KEYBD_NAME, (int) getpid());

	#ifdef DEBUG
	puts("keyboard fifos names:");
	puts(fifo_name_server);
	puts(fifo_name_relauncher);
	#endif

	if(mkfifo(fifo_name_server, 0600) == -1)	// open for reading and writing so that it does not block
	{
		perror("create fifo controller server");
		exit(EXIT_FAILURE);
	}

	if(mkfifo(fifo_name_relauncher, 0600) == -1)	// open for reading and writing so that it does not block
	{
		perror("create fifo controller relauncher");
		exit(EXIT_FAILURE);
	}

	/* create file descriptors for both fifos */
	
	fifo_keybd_server = open(fifo_name_server, O_RDWR);
	if(fifo_keybd_server == -1)
	{
		perror("Open keybd fifo to server");
		exit(EXIT_FAILURE);
	}

	fifo_keybd_relauncher = open(fifo_name_relauncher, O_RDWR);
	if(fifo_keybd_relauncher == -1)
	{
		perror("Open keybd fifo to relauncher");
		exit(EXIT_FAILURE);
	}

	signal(SIGPIPE, SIG_IGN);	// needed so that there is no program termination when write() tries to write to a broken pipe
	signal(SIGINT, SIG_IGN);	// ignore ctrl-c
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
				#ifdef DEBUG
				puts("server received LOG command");
				#endif
				// print log
				break;
			case CONTROLLER_TO_SERVER__TYPE__QUIT:
				// clean memory and quit
				#ifdef DEBUG
				puts("server received QUIT command");
				#endif
				exit(EXIT_SUCCESS);
				break;
			default:
				break;
		}

		free(buf);
		free(msg);
	}
}


void * relauncher_keyboard(void *var)
{
	int len_received;
	char *buf;
	ControllerToServer *msg;

	#ifdef DEBUG
	puts("starting relauncher_keyboard()");
	#endif

	while(1)
	{
		buf = NULL;
		len_received = PROTOrecv(fifo_keybd_relauncher, &buf);
		if(len_received < 0)
		{
			free(buf);
			continue;
		}

		msg = controller_to_server__unpack(NULL, len_received, (uint8_t*) buf);

		switch(msg->type)
		{
			case CONTROLLER_TO_SERVER__TYPE__QUIT:
				// clean memory and quit
				#ifdef DEBUG
				puts("relauncher received QUIT command");
				#endif
				exit(EXIT_SUCCESS);
				break;
			default:
				break;
		}

		free(buf);
		free(msg);
	}
}

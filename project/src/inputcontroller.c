#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include "messages.pb-c.h"
#include "protobufutils.h"
#include "boolean.h"
#include "controllerfifos.h"
#include "define.h"
#include "utils.h"

#define SERVER_COMM_LEN 10

int main(int argc, char **argv)
{
	/*-------- check arguments --------*/
	if(argc != 2)
	{
		printf("Usage: %s <fifo_identifier>\n\n", argv[0]);
		puts("Where <fifo_identifier> is the PID associated with the server");
		printf("e.g: If there is a FIFO with the name server-broadcast-26523\n");
		printf("     input should be: %s 26523\n", argv[0]);
		exit(EXIT_SUCCESS);
	}	
	
	char fifo_name_server[strlen(FIFO_KEYBD_NAME) + strlen("server-") + 8]; // save some space for a PID
	char fifo_name_relauncher[strlen(FIFO_KEYBD_NAME) + strlen("relauncher-") + 8];

	sprintf(fifo_name_server, "%s-server-%s", FIFO_KEYBD_NAME, argv[1]);
	sprintf(fifo_name_relauncher, "%s-relauncher-%s", FIFO_KEYBD_NAME, argv[1]);

	#ifdef DEBUG
	puts(fifo_name_server);
	puts(fifo_name_relauncher);
	#endif

	int fifo_server = open(fifo_name_server, O_RDWR | O_NONBLOCK);
	int fifo_relauncher = open(fifo_name_relauncher, O_RDWR | O_NONBLOCK);
	
	if(fifo_server == -1 || fifo_relauncher == -1)
	{
		perror("Open fifo");	
		exit(EXIT_FAILURE);
	}

	putchar('\n');
	listcommands();
	putchar('\n');

	/*-------- END check arguments --------*/

	signal(SIGPIPE, SIG_IGN);   // needed so that there is no program termination when write() tries to write to a broken pipe

	char comm[SERVER_COMM_LEN];
	boolean valid_command;
	char *buf;
	ControllerToServer msg = CONTROLLER_TO_SERVER__INIT;

	while(1)
	{	

		if(fgets(comm, SERVER_COMM_LEN, stdin)==NULL)
		{
			puts("fgets failed");
		}
		else
		{	
			valid_command = true;
	
			comm[strlen(comm)-1] = 0;	// remove \n

			if(strcmp(comm, "QUIT") == 0)
			{
				//puts("Exiting...");
				msg.type = CONTROLLER_TO_SERVER__TYPE__QUIT;
			}
			else if(strcmp(comm, "LOG") == 0)
			{
				//puts("Log:");
				msg.type = CONTROLLER_TO_SERVER__TYPE__LOG;
			}
			else
			{
				valid_command = false;
				printf("Invalid command '%s'.\n", comm);
			}
			
			if(valid_command)
			{
				buf = malloc(controller_to_server__get_packed_size(&msg));
				if(buf == NULL)
				{
					perror("malloc()");
					exit(EXIT_FAILURE);
				}

				controller_to_server__pack(&msg, (uint8_t*) buf);
				if(PROTOsend(fifo_server, (char*) buf, controller_to_server__get_packed_size(&msg)) != 0)
				{
					perror("Failed to send command to server");
				}

				if(PROTOsend(fifo_relauncher, (char*) buf, controller_to_server__get_packed_size(&msg)) != 0)
				{
					perror("Failed to send command to relauncher");
				}

				free(buf);

				if(msg.type == CONTROLLER_TO_SERVER__TYPE__QUIT)
				{
					char remove_command[100];
					sleep(1);
					sprintf(remove_command, "rm /tmp/server-*%s", argv[1]);
					system(remove_command);
					exit(EXIT_SUCCESS);
				}

			}
		}
	}
}



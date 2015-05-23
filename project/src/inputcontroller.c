#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "messages.pb-c.h"
#include "protobufutils.h"
#include "boolean.h"
#include "controllerfifos.h"
#include "define.h"

#define SERVER_COMM_LEN 10

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("Usage: %s <fifo_identifier>\n\n", argv[0]);
		puts("Where <fifo_identifier> is the PID associated with the server");
		printf("e.g: If there is a FIFO with the name server-broadcast-26523\n");
		printf("     input should be: %s 26523\n", argv[0]);
		exit(EXIT_SUCCESS);
	}	
	
	char fifo_input_server[strlen(FIFO_NAME)+8]; // save some space for a PID
	char comm[SERVER_COMM_LEN];

	sprintf(fifo_input_server, "%s-%s", FIFO_NAME, argv[1]);
	#ifdef DEBUG
	puts(fifo_input_server);
	#endif

	int fifo_input = open(fifo_input_server, O_RDWR | O_NONBLOCK);
	
	if(fifo_input == -1)
	{
		perror("Open fifo");	
		exit(EXIT_FAILURE);
	}

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
				exit(EXIT_SUCCESS);
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
				if(PROTOsend(fifo_input, (char*) buf, controller_to_server__get_packed_size(&msg)) != 0)
				{
					perror("Failed to send command to server");
				}
				free(buf);
			}
		}
	}
}



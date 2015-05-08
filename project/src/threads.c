#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#define SERVER_COMM_LEN 10

// read input from keyboard
void * read_commands()
{
	char comm[SERVER_COMM_LEN];

	while(1)
	{
		if(fgets(comm, SERVER_COMM_LEN, stdin)==NULL)
		{
			puts("fgets failed");
			sleep(2);
		}
		else
		{
			comm[strlen(comm)-1] = 0;	// remove \n

			if(strcmp(comm, "QUIT") == 0)
			{
				puts("Exiting...");
				exit(EXIT_SUCCESS);
			}
			else if(strcmp(comm, "LOG") == 0)
			{
				puts("Log:");
			}
			else
			{
				printf("Invalid command '%s'.\n", comm);
			}
		}
	}
}


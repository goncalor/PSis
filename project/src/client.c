#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGIN_STR "LOGIN"	// username 
#define DISC_STR "DISC"
#define CHAT_STR "CHAT"	// string
#define QUERY_STR "QUERY"	// id_min id_max
#define EXIT_STR "QUIT"


int main()
{
	int should_exit;
	char line[100];
	char command[100];
	char cmd_str_arg[100];
	int  cmd_int_arg1, cmd_int_arg2;

	short is_logged = 0;

	should_exit= 0;



	while(! should_exit)
	{

		fgets(line, 100, stdin);
		if(sscanf(line, "%s", command) == 1)
		{
			if(strcmp(command, LOGIN_STR) == 0)
			{
				if(sscanf(line, "%*s %s", cmd_str_arg) == 1)
				{
					if(is_logged)
					{
						puts("Already logged in. Disconnect to login again.");
					}
					else
					{
						is_logged = 1;

						printf("Sending LOGIN command (%s)\n", cmd_str_arg);
					}

				}
				else
				{
					printf("Invalid LOGIN command\n");
				}
			}
			else if(strcmp(command, DISC_STR)==0)
			{

				is_logged = 0;
				printf("Sending DISconnnect command\n");

			}
			else if(strcmp(command, CHAT_STR)==0)
			{
				if(sscanf(line, "%*s %s", cmd_str_arg) == 1)
				{

					printf("Sending CHAT command (%s)\n", cmd_str_arg);


				}
				else
				{
					printf("Invalid CHAT command\n");
				}
			}
			else if(strcmp(command, QUERY_STR)==0)
			{
				if(sscanf(line, "%*s %d %d", &cmd_int_arg1, &cmd_int_arg2) == 2)
				{

					printf("Sending QUERY command (%d %d)\n", cmd_int_arg1, cmd_int_arg2);

				}
				else
				{
					printf("Invalid QUERY command\n");
				}
			}
			else if(strcmp(command, EXIT_STR)==0)
			{
				should_exit = 1;
				puts("Exiting...");

			}
			else
			{
				printf("Invalid command\n");
			}
		}
		else
		{
			printf("Error in command\n");
		}
	}

	exit(0);
}

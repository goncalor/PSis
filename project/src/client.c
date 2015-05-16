#include "messages.pb-c.h"
#include "TCPlib.h"
#include "inetutils.h"
#include "define.h"
#include "protobufutils.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define LOGIN_STR "LOGIN"	// username 
#define DISC_STR "DISC"
#define CHAT_STR "CHAT"	// string
#define QUERY_STR "QUERY"	// id_min id_max
#define EXIT_STR "QUIT"

int main(int argc, char **argv)
{
	char line[100];
	char command[100];
	char cmd_str_arg[100];
	short should_exit = 0, is_logged = 0;
	unsigned cmd_int_arg1, cmd_int_arg2;
	unsigned ip = atoh("127.0.0.1");
	unsigned short port = 3000;

/*-------- check arguments --------*/

	int opt;

	while((opt=getopt(argc, argv, "p:i:"))!=-1) /*getopt() returns one arg at a time. sets optarg*/
	{
		switch(opt)
		{
			case 'p': port = atoi(optarg); break;
			case 'i': ip = atoh(optarg); break;
			case '?': usage(argv[0]); exit(EXIT_SUCCESS);
		}
	}

	#ifdef DEBUG
	printf("ip: 0x%x\n", ip);
	printf("port: %hu\n", port);
	#endif

/*-------- END check arguments --------*/

	int TCPfd = TCPconnect(ip, port);
	if(TCPfd < 0)
	{
		perror("Failed to create socket or connect()");
		exit(EXIT_FAILURE);
	}


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
						is_logged = login(TCPfd, cmd_str_arg);
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


int login(int fd, char *username)
{
	ClientToServer msg = CLIENT_TO_SERVER__INIT;
	uint8_t *buf;

	msg.type = CLIENT_TO_SERVER__TYPE__LOGIN;
	msg.str = username;
	//msg.has_id_min = 0;
	//msg.has_id_max = 0;

	buf = malloc(client_to_server__get_packed_size(&msg));
	printf("%d\n", client_to_server__get_packed_size(&msg));
	if(buf == NULL)
	{
		perror("malloc in login()");
		exit(EXIT_FAILURE);
	}
	client_to_server__pack(&msg, buf);

	printf("Sending LOGIN command (%s)\n", username);

	if(PROTOsend(fd, (char*) buf, client_to_server__get_packed_size(&msg)) != 0)
	{
		puts("Failed to send message");
		return false;
	}

	return true;
}


void usage(char *exename)
{
	printf("Usage: %s [-i ip] [-p port]\n", exename);
	putchar('\n');
}


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
#include <signal.h>

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

	int TCPfd;
	signal(SIGINT, SIG_IGN);

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
						// create socket and connect to the server
						TCPfd = TCPconnect(ip, port);
						if(TCPfd < 0)
						{
							perror("Failed to create socket or connect()");
							exit(EXIT_FAILURE);
						}

						printf("Sending LOGIN command (%s)\n", cmd_str_arg);
						// send login message and receive confirmation
						is_logged = login(TCPfd, cmd_str_arg);
						if(is_logged == true)
						{
							printf("You are now logged in as '%s'\n", cmd_str_arg);
						}
						else
						{
							puts("Failed to login. Try again with another username");
						}
					}
				}
				else
				{
					printf("Invalid LOGIN command\n");
				}
			}
			else if(strcmp(command, DISC_STR)==0)
			{
				if(is_logged)
				{
					is_logged = false;
					printf("Sending DISConnect command\n");
					disconnect(TCPfd);
				}
				else
					puts("You are not connected");
			}
			else if(strcmp(command, CHAT_STR)==0)
			{
				if(sscanf(line, "%*s %[^\n]", cmd_str_arg) == 1)
				{
					if(!is_logged)
					{
						puts("Please login first");
					}
					else
					{
						printf("Sending CHAT command (%s)\n", cmd_str_arg);
						chat(TCPfd, cmd_str_arg);
					}
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
					if(!is_logged)
					{
						puts("Please login first");
					}
					else
					{
						printf("Sending QUERY command (%d %d)\n", cmd_int_arg1, cmd_int_arg2);
						query(TCPfd, cmd_int_arg1, cmd_int_arg2);
					}
				}
				else
				{
					printf("Invalid QUERY command\n");
				}
			}
			else if(strcmp(command, EXIT_STR)==0)
			{
				should_exit = 1;
				if(is_logged)
					disconnect(TCPfd);
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

	// send login message
	msg.type = CLIENT_TO_SERVER__TYPE__LOGIN;
	msg.str = username;

	buf = malloc(client_to_server__get_packed_size(&msg));
	if(buf == NULL)
	{
		perror("malloc in login()");
		exit(EXIT_FAILURE);
	}
	client_to_server__pack(&msg, buf);
	if(PROTOsend(fd, (char*) buf, client_to_server__get_packed_size(&msg)) != 0)
	{
		puts("Failed to send message");
		free(buf);
		return false;
	}
	free(buf);	// sent information not needed anymore. will reuse the buffer

	// receive login confirmation
	ServerToClient *msgStC;
	buf = NULL;
	int len_received = PROTOrecv(fd, (char**)&buf);

	if(len_received < 0)
	{
		free(buf);
		return false;
	}

	msgStC = server_to_client__unpack(NULL, len_received, (uint8_t*) buf);
	free(buf);
	if(msgStC->has_code && msgStC->code == SERVER_TO_CLIENT__CODE__OK)
	{
		free(msgStC);
		return true;
	}

	free(msgStC);
	return false;
}


void disconnect(int fd)
{
	ClientToServer msg = CLIENT_TO_SERVER__INIT;
	uint8_t *buf;

	// send disconnect message
	msg.type = CLIENT_TO_SERVER__TYPE__DISC;

	buf = malloc(client_to_server__get_packed_size(&msg));
	if(buf == NULL)
	{
		perror("malloc in disconnect()");
		exit(EXIT_FAILURE);
	}
	client_to_server__pack(&msg, buf);
	if(PROTOsend(fd, (char*) buf, client_to_server__get_packed_size(&msg)) != 0)
		puts("Failed to send message");

	free(buf);	// sent information not needed anymore. will reuse the buffer
	TCPclose(fd);
}


void chat(int fd, char *message)
{
	ClientToServer msg = CLIENT_TO_SERVER__INIT;
	uint8_t *buf;

	// send disconnect message
	msg.type = CLIENT_TO_SERVER__TYPE__CHAT;
	msg.str = message;

	buf = malloc(client_to_server__get_packed_size(&msg));
	if(buf == NULL)
	{
		perror("malloc in disconnect()");
		exit(EXIT_FAILURE);
	}
	client_to_server__pack(&msg, buf);
	if(PROTOsend(fd, (char*) buf, client_to_server__get_packed_size(&msg)) != 0)
		puts("Failed to send message");

	free(buf);	// sent information not needed anymore. will reuse the buffer
}


int query(int fd, unsigned first, unsigned last)
{
	ClientToServer msg = CLIENT_TO_SERVER__INIT;
	uint8_t *buf;

	// send disconnect message
	msg.type = CLIENT_TO_SERVER__TYPE__QUERY;
	msg.has_id_min = true;
	msg.has_id_max = true;
	msg.id_min = first;
	msg.id_max = last;

	buf = malloc(client_to_server__get_packed_size(&msg));
	if(buf == NULL)
	{
		perror("malloc in disconnect()");
		exit(EXIT_FAILURE);
	}
	client_to_server__pack(&msg, buf);
	if(PROTOsend(fd, (char*) buf, client_to_server__get_packed_size(&msg)) != 0)
		puts("Failed to send message");
	free(buf);	// sent information not needed anymore. will reuse the buffer

	// receive query results
	ServerToClient *msgStC;
	buf = NULL;
	int len_received = PROTOrecv(fd, (char**)&buf);

	if(len_received < 0)
	{
		free(buf);
		return false;
	}

	msgStC = server_to_client__unpack(NULL, len_received, (uint8_t*) buf);
	free(buf);

	int i;
	if(msgStC->n_str >= 1)
		for(i=0; i < msgStC->n_str; i++)
			printf("%d %s\n", first+i, msgStC->str[i]);
	else
		puts("No messages in the specified range");

	free(msgStC);
	return i;
}


void usage(char *exename)
{
	printf("Usage: %s [-i ip] [-p port]\n", exename);
	putchar('\n');
}


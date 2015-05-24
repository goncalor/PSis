#include <stdio.h>

void usage(char *exename)
{
	printf("Usage: %s [-p port]\n", exename);
	putchar('\n');
}

void listcommands()
{
	puts("Command list:");
	puts("\t\tLOG - prints the log");
	puts("\t\tQUIT - terminates the server");
}

void listcommands_client()
{
	puts("Command list:");
	puts("\tLOGIN <username>  - logs you in");
	puts("\tCHAT <message>    -  send a chat");
	puts("\tDISC              - disconnects you from the server");
	puts("\tQUERY <nr1> <nr2> - asks for messages between nr1 and nr2");
	puts("\tQUIT              - terminates the program");
}

void version(char *nr)
{
	printf("server version %s\n", nr);
}

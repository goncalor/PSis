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

void version(char *nr)
{
	printf("server version %s\n", nr);
}

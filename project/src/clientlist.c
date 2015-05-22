#include "clientlist.h"
#include "list.h"
#include "messages.pb-c.h"
#include "protobufutils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct clientinfo {
	int fd;
	char *username;
} clientinfo;

/* returns 0 if the usernames in 'ci1' and 'ci2' are equal */
Item CLcompare(Item ci1, Item ci2)
{
	if(strcmp(((clientinfo*)ci1)->username, ((clientinfo*)ci2)->username) == 0)
		return (Item) 0;
	else
		return (Item) -1;
}

void CLfree(Item ci)
{
	free(((clientinfo*)ci)->username);
	free(ci);
}

clientlist * CLinit()
{
	return LSTinit();
}

/* returns 1 if the client was added, that is there was no
 * previous client with the username 'username'. 
 * 'lst' is updated only if the client is added */
int CLadd(clientlist **lst, int fd, char *username)
{
	clientlist *aux;
	clientinfo *ci = malloc(sizeof(clientinfo));
	if(ci==NULL)
	{
		perror("CLadd()");
		exit(EXIT_FAILURE);
	}

	ci->fd = fd;
	ci->username = strdup(username);

	for(aux=*lst; aux != NULL; aux = LSTfollowing(aux))
	{
		if(LSTapply(aux, CLcompare, (Item) ci) == 0)
		{
			CLfree(ci);
			return 0;
		}
	}

	*lst = LSTadd(*lst, (Item) ci);

	return 1;
}


clientlist * CLremove(clientlist *lst, int fd)
{
	clientlist *aux, *aux2, *aux3;

	aux2 = NULL;
	for(aux=lst; aux != NULL; aux = LSTfollowing(aux))
	{
		// found the client we want to remove
		if(((clientinfo*)LSTgetitem(aux))->fd == fd)
		{
			aux3 = LSTremove(aux2, aux, CLfree);
			if(aux2 == NULL)
				return aux3;
			return lst;
		}

		aux2 = aux;
	}

	return lst;
}


/* broadcasts 'msg' of size 'len' to all the clients in 'lst'.
 * returns the number of messages that failed to be sent. this
 * means it returns 0 on success */
int CLbroadcast(clientlist *lst, char *msg, unsigned len)
{
	clientlist *aux;
	int err=0;

	for(aux=lst; aux != NULL; aux = LSTfollowing(aux))
	{
		if(PROTOsend(((clientinfo*)LSTgetitem(aux))->fd, msg, len) != 0)
		{
			puts("Failed to reply to login message");
			err++;
		}
	}

	return err;
}


void CLdestroy(clientlist *lst)
{
	LSTdestroy(lst, free);
}


void CLprint(clientlist *lst)
{
	clientlist *aux;

	if(lst==NULL)
		puts("-> (empty)");

	for(aux=lst; aux != NULL; aux = LSTfollowing(aux))
	{
		printf("-> %s\n", ((clientinfo*)LSTgetitem(aux))->username);
	}
}

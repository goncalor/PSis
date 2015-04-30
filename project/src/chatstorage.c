#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chatstorage.h"

#define CS_STEP 1024

struct chatdb {
	char **messages;
	unsigned nr_messages;
	unsigned max_messages;
};

chatdb * CSinit(void)
{
	chatdb * new = (chatdb *) malloc(sizeof(chatdb));;
	if(new == NULL)
	{
		perror("allocate new chatdb");
		exit(EXIT_FAILURE);
	}

	new->nr_messages = 0;
	new->nr_messages = CS_STEP;
	new->messages = malloc(CS_STEP*sizeof(char *));
	if(new->messages == NULL)
	{
		perror("allocate new chatdb.messages");
		exit(EXIT_FAILURE);
	}

	return new;
}


void CSstore(chatdb *db, char *message)
{
	if(db->nr_messages == db->max_messages)
	{
		// verify realloc errors
		db->max_messages = db->max_messages + CS_STEP;
		db->messages = realloc(db->messages, db->max_messages);
	}
		
	db->messages[db->nr_messages] = malloc((strlen(message)+1)*sizeof(char));
	if(db->messages == NULL)
	{
		perror("allocate new message");
		exit(EXIT_FAILURE);
	}
	strcpy(db->messages[db->nr_messages], message);
	db->nr_messages++;
}


// returns an array with the strings from first to last (including)
// the last position in the array is NULL terminated
// first = 0 corresponds to the first stored chat message
// if first = last at most one message is returned
// if the asked indexes make no sense NULL is returned
char ** CSquery(chatdb *db, unsigned first, unsigned last)
{
	char **chunk;
	unsigned i;

	if(last >= db->nr_messages)
		last = db->nr_messages - 1;

	if(last < first)
		return NULL;

	chunk = malloc((last-first+2)*sizeof(char *));	// extra position needed for NULL termination
	if(chunk == NULL)
	{
		perror("allocate chunk in CSquery()");
		exit(EXIT_FAILURE);
	}

	for(i=0; i <= last-first; i++)
		chunk[i] = db->messages[first+i];

	chunk[i] = NULL;	// NULL terminate the new array

	return chunk;
}

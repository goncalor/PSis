#include "chatstorage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct chatdb {
	char **messages;
	unsigned nr_messages;	// the next free index in 'messages'
	unsigned max_messages;	// the length of chatdb.'messages' array
};


// creates and inits a new chat database
// returns a pointer to that new database. NULL on error
chatdb * CSinit(void)
{
	chatdb * new = (chatdb *) malloc(sizeof(chatdb));;
	if(new == NULL)
	{
		perror("allocate new chatdb");
		return NULL;
	}

	new->nr_messages = 0;
	new->max_messages = CS_STEP;
	new->messages = malloc(CS_STEP*sizeof(char *));
	if(new->messages == NULL)
	{
		perror("allocate new chatdb.messages");
		return NULL;
	}

	return new;
}


// stores "message" in the chat database "db"
// returns 0 on success, negative on error
int CSstore(chatdb *db, char *message)
{
	if(db->nr_messages == db->max_messages)
	{
		db->max_messages = db->max_messages + CS_STEP;
		db->messages = realloc(db->messages, db->max_messages*sizeof(char *));
		if(db->messages == NULL)
		{
			perror("reallocate messages array");
			return -1;
		}
	}
		
	db->messages[db->nr_messages] = malloc((strlen(message)+1)*sizeof(char));
	if(db->messages[db->nr_messages] == NULL)
	{
		perror("allocate new message");
		return -2;
	}
	strcpy(db->messages[db->nr_messages], message);
	db->nr_messages++;

	return 0;
}


// returns an array with the strings from first to last (including)
// returns NULL if there is an error
// the last position in the array is NULL terminated
// first = 0 corresponds to the first stored chat message
// if first = last at most one message is returned
// returns can return an empty array (first position set to NULL)
char ** CSquery(chatdb *db, unsigned first, unsigned last)
{
	char **chunk;
	unsigned i;

	last++;	// we will regard last as exclusive inside the function
	if(last > db->nr_messages)
		last = db->nr_messages;

	if(last < first)
	{
		chunk = malloc(sizeof(char *));
		if(chunk == NULL)
		{
			perror("allocate chunk in CSquery()");
			return NULL;
		}
		*chunk = NULL;	// chunk will have only one position and it will be NULL
	}
	else
	{
		chunk = malloc((last-first+1)*sizeof(char *));	// extra position needed for NULL termination
		if(chunk == NULL)
		{
			perror("allocate chunk in CSquery()");
			return NULL;
		}

		for(i=0; i < last-first; i++)
			chunk[i] = db->messages[first+i];

		chunk[i] = NULL;	// NULL terminate the new array
	}

	return chunk;
}


/*Returns the number of messages in the ChatDataBase*/
int CSsize(chatdb *ptr)
{
	return ptr->nr_messages;
}

/*Returns the length of chatdb.'messages'*/
int CSlength(chatdb *ptr)
{
	return ptr->max_messages;
}

void CSdestroy(chatdb *db)
{
	int i;

	for(i=0; i < db->nr_messages; i++)
		free(db->messages[i]);

	free(db->messages);
	free(db);
}


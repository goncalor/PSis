#include "storyserver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_LEN 100

int main(int argc, char **argv){
	int server_fifo, story_fifo;
	message m;
	char fifo_name[100];
	char *story;
	int curr_size, size_read;

	if(argc != 2)
	{
		printf("Usage: %s fifo_nr\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* open FIFO */

	server_fifo = open(SERVER_FIFO, O_WRONLY, 00200);
	if(server_fifo == -1){
		perror("error opening");
		exit(EXIT_FAILURE);
	}

	printf("message: ");
	fgets(m.buffer, MESSAGE_LEN, stdin);
	m.n_fifo = atoi(argv[1]);

	/* create fifo  named SERVER_FIFO_nr */
	sprintf(fifo_name, "%s_%d", SERVER_FIFO, m.n_fifo);
	if(mkfifo(fifo_name, 0600) != 0){
		perror("create incoming fifo"); 
		exit(EXIT_FAILURE);
	}

	story_fifo = open(fifo_name, O_WRONLY, 00400);
	if(story_fifo == -1){
		perror("error opening client fifo");
		exit(EXIT_FAILURE);
	}

	/* write message */

	/* this should loop until all message has been sent or an error occurs */
	if(write(server_fifo, &m, sizeof(message)) == -1)
	{
		perror("error writing");
		exit(EXIT_FAILURE);
	}

	/* receive story */

	story = malloc(BUF_LEN);

	curr_size = 0;
	while((size_read = read(server_fifo, story+curr_size, BUF_LEN)) > 0)
	{
		curr_size += size_read;

// finish this
//		if(curr_size + BUF_LEN + 1 > )
//			story = realloc(story, strlen(story)+size_read+sizeof(char));
	}

	if(size_read == -1)
	{
		perror("error reading");
		exit(EXIT_FAILURE);			
	}

	printf("OK\n");
	exit(0);
}

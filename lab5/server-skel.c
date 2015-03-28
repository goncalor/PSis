#include "storyserver.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(){
	int server_fifo, response_fifo;
	char fifo_name[MESSAGE_LEN];
	message m;
	char * story;
	int size_read;

	story = malloc(sizeof(char));
	story[0] = '\0';


	if(unlink(SERVER_FIFO) == -1)
	{
		perror("unlink");
	}

	/* create fifo  named SERVER_FIFO */
	if(mkfifo(SERVER_FIFO, 0600) != 0){
		perror("create fifo "); 
		exit(EXIT_FAILURE);
	}

	/*Open FIFO*/
	server_fifo = open(SERVER_FIFO, O_RDONLY, 00400);
	if(server_fifo == -1){
		perror("error opening");
		exit(EXIT_FAILURE);
	}

	printf("FIFO created and opened.\n");

	/* pen for reading */
	while(1){
		/* read message and process it */

		if((size_read = read(server_fifo, &m, sizeof(message))) > 0)
		{
			printf("%d: %s\n", m.n_fifo, m.buffer);

			sprintf(fifo_name, "%s_%d", SERVER_FIFO, m.n_fifo);
			response_fifo = open(fifo_name, O_WRONLY, 00200);
			if(response_fifo == -1){
				perror("error opening client fifo");
				exit(EXIT_FAILURE);
			}

			m.buffer[strlen(m.buffer)-1] = ' ';	// substitute \n for a space
			story = realloc(story, strlen(story)+size_read+sizeof(char));

			strcat(story, m.buffer);
			printf("story: %s\n", story);

			// send story to sender

			if(write(response_fifo, &story, strlen(story)+sizeof(char)) == -1)
			{
				perror("error writing story");
				exit(EXIT_FAILURE);
			}

		}
		else if(size_read == -1)
		{
			perror("error reading");
			exit(EXIT_FAILURE);			
		}

	}



	printf("OK\n");
	exit(0);

}

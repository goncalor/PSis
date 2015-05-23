#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "controllerfifos.h"
#include "define.h"

void setup_server_controller()
{
	char fifo_input_server[strlen(FIFO_NAME)+8]; // save some space for a PID

	sprintf(fifo_input_server, "%s-%d", FIFO_NAME, (int) getpid());

	#ifdef DEBUG
	puts(fifo_input_server);
	#endif

	if(mkfifo(fifo_input_server, 0600) == -1)	// open for reading and writing so that it does not block
	{
		perror("create fifo controller");
		exit(EXIT_FAILURE);
	}
}


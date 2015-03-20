#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include "heartbeat.h"
#include <errno.h>

int main(int argc, char * argv[]){
	
	mesg buffer;
	struct mq_attr queue_attr;
	mqd_t mq;

	queue_attr.mq_maxmsg = MAX_TASKS;
	queue_attr.mq_msgsize = sizeof(mesg);

	mq = mq_open(MQ_NAME, O_RDONLY | O_CREAT, 0600, &queue_attr);
	if(mq == -1)
	{
		perror("Error opening queue");
		exit(-1);
	}

	while(1)
	{
		if(mq_receive(mq, (char *) &buffer, sizeof(mesg), NULL) == -1)
			perror("mq receive");

		printf("%d\n", buffer.id);
	}

	exit(EXIT_SUCCESS);
}

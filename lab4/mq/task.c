#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include "heartbeat.h"

int main(int argc, char * argv[]){
	int task_id = -1;

	mesg buffer;
	struct mq_attr queue_attr;
	mqd_t mq;

	if(argc > 1){
		sscanf(argv[1], "%d", &task_id);
	}

	if (task_id ==-1){
		printf("Task identifier is missing\n");
		exit(1);
	}else{
		if(task_id >= MAX_TASKS)
		{
			printf("Task_id must be less than %d", MAX_TASKS);
			exit(EXIT_FAILURE);
		}
		printf("Taks_id %d\n", task_id);
	}

	queue_attr.mq_maxmsg = MAX_TASKS;
	queue_attr.mq_msgsize = sizeof(mesg);

	mq = mq_open(MQ_NAME, O_WRONLY | O_CREAT, 0600, &queue_attr);
	if(mq == -1)
	{
		perror("Error opening queue");
		exit(-1);
	}


	while(1){
		printf("sending heartbeat\n");

		buffer.id = task_id;
		if(mq_send(mq, (char *) &buffer, sizeof(mesg), 1) == -1)
		{
			perror("Error sending to queue");
			exit(-1);
		}

		sleep(TICK_TIME);
	}

	exit(0);
}

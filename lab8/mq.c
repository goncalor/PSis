#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include "heartbeat.h"

void MQcreate()
{
	mq = mq_open(MQ_NAME, O_WRONLY | O_CREAT, 0600, &queue_attr);
	if(mq == -1)
	{
		perror("Error opening queue");
		exit(-1);
	}
}


void MQsendbeat()
{

	mesg buffer;
	struct mq_attr queue_attr;
	mqd_t mq;


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

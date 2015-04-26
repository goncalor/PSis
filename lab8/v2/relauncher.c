#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "heartbeat.h"

void launch()
{
	pid_t pid;

	puts("relaunching server...");
	pid = fork();

	if(pid == 0)
	{
		//child
		if(execlp("./server", "server", "--no-relaunch", NULL) == -1)
		{
			perror("execve");
			exit(-1);
		}
	}
}

void * receive_beat(void *mqd)
{
	mesg buffer;
	mqd_t mq = *(mqd_t*) mqd;
	struct timespec timeout;

	buffer.id = 0;
	while(1)
	{
		clock_gettime(CLOCK_REALTIME_COARSE, &timeout);
		timeout.tv_sec += 3;	// wait max 3 seconds for a server ping

		if(mq_timedreceive(mq, (char *) &buffer, sizeof(mesg), NULL, &timeout) == -1)
		{
			perror("Error receiving from queue");
			if(errno == ETIMEDOUT)
			{
				pthread_exit(NULL);
			}
		}
	}

}

int main()
{
	int status;

	/* open a message queue */
	mqd_t mq;
	struct mq_attr queue_attr;

	queue_attr.mq_maxmsg = MAX_TASKS;
	queue_attr.mq_msgsize = sizeof(mesg);

	mq = mq_open(MQ_NAME, O_RDONLY, 0600, &queue_attr);
	if(mq == -1)
	{
		perror("Error opening queue");
		exit(-1);
	}

	pthread_t thread_mqrcv;
	pthread_create(&thread_mqrcv, NULL, receive_beat, &mq);
	pthread_join(thread_mqrcv, NULL);

	/* at this point we know the server has crashed once and 
	 * therefore the relauncher is now the server's parent.
	 * so now we can use wait() to see if the child dies.
	 * if it does just create a new child */

	while(1)
	{
		launch();
		wait(&status);
		printf("wait() status %d\n", status);
	}

	return 0;
}

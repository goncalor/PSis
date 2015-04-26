#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <mqueue.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include "heartbeat.h"

int i = 0;

void relauncher_launch()
{
	pid_t pid;
	pid = fork();

	if(pid == 0)
	{
		//child
		if(execlp("./relauncher", "relauncher", NULL) == -1)
		{
			perror("execve could not start relauncher");
			kill(getppid(), SIGTERM);
			exit(-1);
		}
	}
	// parent simply exits the function
}

void * thread_code(void *arg){
	int value = *((int *)arg);

	while(1)
	{
		sleep(value);
		printf("Thread %d %d\n", value, i);
		i++;
	}

	pthread_exit(NULL);
}

void * send_beat(void *mqd)
{
	mesg buffer;
	mqd_t mq = *(mqd_t*) mqd;

	buffer.id = 0;
	while(1)
	{
		printf("sending heartbeat\n");
		if(mq_send(mq, (char *) &buffer, sizeof(mesg), 1) == -1)
		{
			perror("Error sending to queue");
		}
		sleep(2);
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	pthread_t thread_id_1;
	pthread_t thread_id_2;
	pthread_t thread_send_beat;
	void * res;
	int arg_1;
	int arg_2;

	if(!(argc == 2 && strcmp(argv[1], "--no-relaunch") == 0))
	{
		relauncher_launch();

		/* open a message queue */
		mqd_t mq;
		struct mq_attr queue_attr;

		queue_attr.mq_maxmsg = MAX_TASKS;
		queue_attr.mq_msgsize = sizeof(mesg);

		mq = mq_open(MQ_NAME, O_WRONLY | O_CREAT, 0600, &queue_attr);
		if(mq == -1)
		{
			perror("Error opening queue");
			exit(-1);
		}

		/* start thread to send heartbeats */

		pthread_create(&thread_send_beat, NULL, send_beat, &mq);
		printf("New thread %d\n", (int) thread_send_beat);
	}

	/* server tasks */

	arg_1 = 1;
	pthread_create(&thread_id_1, NULL, thread_code, &arg_1);
	printf("New thread %d\n", (int) thread_id_1);

	arg_2 = 2;	
	pthread_create(&thread_id_2, NULL, thread_code, &arg_2);
	printf("New thread %d\n", (int) thread_id_2);

	getchar();

	pthread_join(thread_id_1, &res);
	pthread_join(thread_id_2, &res);
	exit(0);
}

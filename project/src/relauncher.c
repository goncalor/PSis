#include "crashrecovery.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void relauncher(void)
{

	// crash recovery thereads
	pthread_t thread_send_fifo;
	pthread_create(&thread_send_fifo, NULL, CRrelauncher_write, NULL);

	pthread_t thread_rcv_fifo;
	pthread_create(&thread_rcv_fifo, NULL, CRrelauncher_read, NULL);

	/* thread joins */
	sleep(10000);
	pthread_join(thread_send_fifo, NULL);
	pthread_join(thread_rcv_fifo, NULL);
	puts("relauncher function ended");
}



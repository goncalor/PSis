#include "crashrecovery.h"
#include "threads.h"
#include "TCPlib.h"
#include "define.h"
#include "define.h"
#include "messages.pb-c.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUF_LEN 1024*1024

void server(void)
{
	int newfd;
	char buf[BUF_LEN];

	// keyboard management theread
	pthread_t thread_keyboad;
	pthread_create(&thread_keyboad, NULL, read_commands, NULL);

	// crash recovery thereads
	pthread_t thread_send_fifo;
	pthread_create(&thread_send_fifo, NULL, CRserver_write, NULL);

	pthread_t thread_rcv_fifo;
	pthread_create(&thread_rcv_fifo, NULL, CRserver_read, NULL);
	
	// accepts 
	newfd = TCPaccept(TCPfd);

	memset(&buf, 0, BUF_LEN);
	TCPrecv(newfd, (char*)&buf, BUF_LEN);


	ClientToServer *msg;
	msg = client_to_server__unpack(NULL, BUF_LEN, (uint8_t*) &buf);

	printf("%s\n", msg->str);

	client_to_server__free_unpacked(msg, NULL);


	/* thread joins */
	sleep(10000);
	pthread_join(thread_keyboad, NULL);	// wait for thread_keyboad termination
	pthread_join(thread_send_fifo, NULL);
	pthread_join(thread_rcv_fifo, NULL);
	puts("server function ended");
}

#include "crashrecovery.h"
#include "controllerfifos.h"
#include "messages.pb-c.h"
#include "protobufutils.h"
#include "define.h"
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

	pthread_t thread_killzombies;
	pthread_create(&thread_killzombies, NULL, CRkillzombies, NULL);

	/* create thread for keyboard */
	pthread_t thread_keybd;
	pthread_create(&thread_keybd, NULL, relauncher_keyboard, NULL);


	/* thread joins */
	pthread_join(thread_send_fifo, NULL);
	pthread_join(thread_rcv_fifo, NULL);
	pthread_join(thread_killzombies, NULL);
	pthread_join(thread_keybd, NULL);
	#ifdef DEBUG
	puts("relauncher function ended");
	#endif
}


void * relauncher_keyboard(void *var)
{
	int len_received;
	char *buf;
	ControllerToServer *msg;

	#ifdef DEBUG
	puts("starting relauncher_keyboard()");
	#endif

	while(1)
	{
		buf = NULL;
		len_received = PROTOrecv(fifo_keybd_relauncher, &buf);
		if(len_received < 0)
		{
			free(buf);
			continue;
		}

		msg = controller_to_server__unpack(NULL, len_received, (uint8_t*) buf);

		switch(msg->type)
		{
			case CONTROLLER_TO_SERVER__TYPE__QUIT:
				// clean memory and quit
				puts("Received QUIT command. Relauncher is closing...");
				exit(EXIT_SUCCESS);
				break;
			default:
				break;
		}

		free(buf);
		free(msg);
	}
}

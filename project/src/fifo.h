#ifndef _FIFO_H
#define _FIFO_H

typedef struct fifo_msg {
	char fifo_write[40];
	char fifo_read[40];
	void (*func) (struct fifo_msg *);
} fifo_msg;

#define FIFO_BUF 3
#define TIMEOUT 9
#define FIFO_NAME_SERVER "/tmp/psis-from-server"
#define FIFO_NAME_RELAUNCHER "/tmp/psis-from-relauncher"

#endif

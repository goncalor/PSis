#ifndef _FIFO_H
#define _FIFO_H

#define FIFO_BUF 6
#define TIMEOUT 10	// don't reduce bellow 2
#define FIFO_NAME_SERVER "/tmp/psis-from-server"
#define FIFO_NAME_RELAUNCHER "/tmp/psis-from-relauncher"

int fifo_server;
int fifo_relauncher;

void CRsetup();
void * CRserver_read(void *var);
void * CRrelauncher_read(void *var);
void * CRserver_write(void *var);
void * CRrelauncher_write(void *var);

#endif

#ifndef _CONTROLLERFIFOS_H
#define _CONTROLLERFIFOS_H

#define FIFO_KEYBD_NAME "/tmp/server-keyboard"

int fifo_keybd_server;
int fifo_keybd_relauncher;

void setup_controllers();
void * relauncher_keyboard(void *var);

#endif

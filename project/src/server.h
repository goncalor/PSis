#ifndef SERVER_H
#define SERVER_H

#include "messages.pb-c.h"

#define FIFO_NAME_BROADCAST "/tmp/server"	// a PID will be appended

void server(void);
void * incoming_connection(void *fd);
void * broadcast_chat(void *arg);
int manage_login(int fd, ClientToServer *msg, int loggedin);
void manage_disconnect(int fd, int loggedin);
void manage_query(int fd, ClientToServer *msg, int loggedin);
void manage_chat(int fd, ClientToServer *msg, int loggedin);

#endif

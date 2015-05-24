#ifndef SERVER_H
#define SERVER_H

#include "messages.pb-c.h"

#define FIFO_NAME_BROADCAST "/tmp/server-broadcast"	// a PID will be appended

void server(void);
void * incoming_connection(void *fd);
void * broadcast_chat(void *arg);
void * server_keyboard(void *var);
int manage_login(int fd, ClientToServer *msg, int loggedin, char **username);
void manage_disconnect(int fd, int loggedin, char *username);
void manage_query(int fd, ClientToServer *msg, int loggedin, char *username);
void manage_chat(int fd, ClientToServer *msg, int loggedin, char *username);

#endif

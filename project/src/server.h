#ifndef SERVER_H
#define SERVER_H

#include "messages.pb-c.h"

void server(void);
void * incoming_connection(void *fd);
int manage_login(int fd, ClientToServer *msg, int loggedin);
void manage_disconnect(int fd, int loggedin);
void manage_query(int fd, ClientToServer *msg, int loggedin);
void manage_chat(int fd, ClientToServer *msg, int loggedin);

#endif

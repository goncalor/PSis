#ifndef SERVER_H
#define SERVER_H

void server(void);
void * incoming_connection(void *fd);
int manage_login(int fd, int loggedin);

#endif

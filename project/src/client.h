#ifndef _CLIENT_H
#define _CLIENT_H

void usage_client(char *exename);
int login(int fd, char *username);
void disconnect(int fd);
int query(int fd, unsigned first, unsigned last);
void chat(int fd, char *message);
int receive_chat(int fd);

#endif

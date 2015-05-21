#ifndef _CLIENT_H
#define _CLIENT_H

void usage(char *exename);
int login(int fd, char *username);
void disconnect(int fd);
int query(int fd, unsigned first, unsigned last);
void chat(int fd, char *message);
void receive_chat(int fd);

#endif

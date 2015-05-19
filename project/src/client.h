#ifndef _CLIENT_H
#define _CLIENT_H

void usage(char *exename);
int login(int fd, char *username);
void disconnect(int fd);

#endif

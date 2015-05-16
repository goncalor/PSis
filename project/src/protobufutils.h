#ifndef PROTOBUFUTILS_H
#define PROTOBUFUTILS_H

int PROTOsend(int fd, char *message, unsigned len);
int PROTOrecv(int fd, char **message);

#endif

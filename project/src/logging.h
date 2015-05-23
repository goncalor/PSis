#ifndef _LOGGING_
#define _LOGGING_

int LOGcreate(char *name, char *ext);
int LOGadd(int fd, int nr, char *line);

#endif

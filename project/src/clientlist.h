#ifndef _CLIENTLIST_H
#define _CLIENTLIST_H

#include "list.h"

typedef list clientlist;

clientlist * CLinit();
int CLadd(clientlist **lst, int fd, char *username);
clientlist * CLremove(clientlist *lst, int fd);
void CLdestroy(clientlist *lst);
void CLprint(clientlist *lst);

#endif

#ifndef _CHATSTORAGE_H
#define _CHATSTORAGE_H

typedef struct chatdb chatdb;

chatdb * CSinit(void);
void CSstore(chatdb *db, char *message);
char ** CSquery(chatdb *db, unsigned first, unsigned last);

#endif

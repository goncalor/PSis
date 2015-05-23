#ifndef chat_storage_H
#define chat_storage_H

#define CS_STEP 1024

typedef struct chatdb chatdb;

chatdb * CSinit(void);
int CSstore(chatdb *db, char *message);
char ** CSquery(chatdb *db, unsigned first, unsigned last);
void CSdestroy(chatdb *db);
int CSsize(chatdb *ptr);
int CSlength(chatdb *ptr);


#endif // chat_storage_H

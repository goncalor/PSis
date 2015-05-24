#ifndef _DEFINE_H
#define _DEFINE_H

#include <pthread.h>

//#define DEBUG

int TCPfd_global;
int LOGfd_global;

pthread_mutex_t mutex_clist;
pthread_mutex_t mutex_chatdb;
pthread_mutex_t mutex_log;

#endif

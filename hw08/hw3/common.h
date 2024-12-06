#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>

#define BUFFER_SIZE 1024

extern char message[BUFFER_SIZE];
extern int hasRequest;

extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

#endif


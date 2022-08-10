#ifndef SHARED_RESOURCES
#define SHARED_RESOURCES
#include <pthread.h>

extern pthread_t pthServerChild;
extern pthread_mutex_t mutex;
extern int clientFd;


#endif
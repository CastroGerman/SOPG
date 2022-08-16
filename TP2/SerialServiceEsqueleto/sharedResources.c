#include "sharedResources.h"

/* Asumimos un solo cliente TCP y nos tomamos la
libertad de no implementar una cola donde llevar el registro
ni un manejo dinamico apropiado. En su lugar, para cumplir, 
aplicamos la fuerza bruta con recursos globales. */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int clientFd = 0;



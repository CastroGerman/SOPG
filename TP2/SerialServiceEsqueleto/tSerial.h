#ifndef T_SERIAL
#define T_SERIAL

#define SERIAL_BUFFER_SIZE 200

void serialThread_closeResources(void);
void* serialThread(void *);

#endif
#ifndef T_SERVER
#define T_SERVER

#define SOCKET_IP "127.0.0.1"
#define SOCKET_PORT 10000
#define SOCKET_BUFFER_SIZE 200

void *serverThread(void *args);
void clientThread(void);
void serverThread_closeResources(void);
void clientThread_closeResources(void);
#endif
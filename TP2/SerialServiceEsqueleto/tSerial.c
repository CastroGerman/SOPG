#include "tSerial.h"
#include "SerialManager.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include "sharedResources.h"

void* serialThread(void *){
    printf("Comienza hilo de comunicacion serial\n");
    char serialBuffer[SERIAL_BUFFER_SIZE];
    int bytesRead = 0;

    if(serial_open(1, 115200) != 0){
        printf("Error abriendo puerto serie\n");
    }

    while (1)
    {
        if((bytesRead = serial_receive(serialBuffer, SERIAL_BUFFER_SIZE-1)) > 0){
            serialBuffer[bytesRead] = '\0';
            printf("Se lee del puerto serie: %s", serialBuffer);
            pthread_mutex_lock(&mutex);
            if (send(clientFd, serialBuffer, bytesRead, MSG_DONTWAIT) == -1) {
               printf("Error al escribir mensaje en el socket. No hay conexión establecida.\n");     
            }
            pthread_mutex_unlock(&mutex);
        }
        sleep(1);
    }
    
    serial_close();
    return NULL;
}

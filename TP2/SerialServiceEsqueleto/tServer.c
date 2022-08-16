#include "tServer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "SerialManager.h"
#include "sharedResources.h"

static int sockFd = 0;
void serverThread_closeResources(void){
    if (sockFd != 0) close(sockFd);
}

static int connFd = 0;
void clientThread_closeResources(void){
    if (connFd != 0) close(connFd);
}

void *serverThread(void *args){
    printf("Comienza hilo de TCP server\n");
    int addrLen = 0;
    struct sockaddr_in servAddr, cliAddr;
    bzero(&servAddr, sizeof(struct sockaddr_in));
    bzero(&cliAddr, sizeof(struct sockaddr_in));
    
    if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("No se pudo crear el socket\n");
        exit(EXIT_FAILURE);
    } 

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(SOCKET_IP);
    servAddr.sin_port = htons(SOCKET_PORT);

    if (bind(sockFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1) {
        close(sockFd);
        printf("No se pudo abrir el puerto, verificar que no esté en uso.\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sockFd, 5) == -1) {
        printf("Error al escuchar el puerto.\n");
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        connFd = 0;
        addrLen = sizeof(struct sockaddr_in);
        if ((connFd = accept(sockFd, (struct sockaddr *)&cliAddr, &addrLen)) == -1) {
            printf("Error al intentar conectarse al puerto.\n");
            exit(EXIT_FAILURE);
        }

        char ipClient[32];
        inet_ntop(AF_INET, &cliAddr.sin_addr, ipClient, sizeof(ipClient));
        printf("Se recibió una conexión desde: %s : %d\n", ipClient, cliAddr.sin_port);


        clientThread();
    }
    close(sockFd);
    exit(EXIT_SUCCESS);
}

void clientThread(void){
    pthread_mutex_lock(&mutex);
    clientFd = connFd;
    pthread_mutex_unlock(&mutex);
    printf("Comienza hilo de TCP client con file descriptor = %d\n", connFd);

    char socketBuffer[SOCKET_BUFFER_SIZE];
    bzero(socketBuffer, SOCKET_BUFFER_SIZE);
    int bytesRead = 0;

    while (1) {
        if ((bytesRead = read(connFd, socketBuffer, SOCKET_BUFFER_SIZE - 1)) <= 0) {
            printf("Error al leer mensaje del socket\n");
            close(connFd);
            return;
        }
        socketBuffer[bytesRead] = '\0';
        printf("SERVER -- Se leyeron %d bytes: %s\n", bytesRead, socketBuffer);
        serial_send(socketBuffer, bytesRead);
    }
    
    close(connFd);
    exit(EXIT_SUCCESS);
}
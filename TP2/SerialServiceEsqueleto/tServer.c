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

void *serverThread(void *args){
    printf("Comienza hilo de TCP server\n");
    int sockFd = 0, addrLen = 0;
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
        int * connFd = (int *)malloc(sizeof(int));
        addrLen = sizeof(struct sockaddr_in);
        if ((*connFd = accept(sockFd, (struct sockaddr *)&cliAddr, &addrLen)) == -1) {
            printf("Error al intentar conectarse al puerto.\n");
            exit(EXIT_FAILURE);
        }

        char ipClient[32];
        inet_ntop(AF_INET, &cliAddr.sin_addr, ipClient, sizeof(ipClient));
        printf("Se recibió una conexión desde: %s : %d\n", ipClient, cliAddr.sin_port);

        pthread_t pthClient;
        int retPthClient = -1;    
        retPthClient = pthread_create(&pthClient, NULL, clientThread, connFd);
        if (retPthClient != 0) {
            printf("Error al crear thread de cliente TCP\n");
        }
    }
    close(sockFd);
    exit(EXIT_SUCCESS);
}

void *clientThread(void *args){
    pthread_detach(pthread_self());
    int connFd = *(int *)args;
    pthread_mutex_lock(&mutex);
    clientFd = *(int *)args;
    pthread_mutex_unlock(&mutex);
    free(args);
    printf("Comienza hilo de TCP client con file descriptor = %d\n", connFd);

    char socketBuffer[SOCKET_BUFFER_SIZE];
    bzero(socketBuffer, SOCKET_BUFFER_SIZE);
    int bytesRead = 0;

    while (1) {
        if ((bytesRead = read(connFd, socketBuffer, SOCKET_BUFFER_SIZE - 1)) <= 0) {
            printf("Error al leer mensaje del socket\n");
            close(connFd);
            pthread_exit(NULL);
        }
        socketBuffer[bytesRead] = '\0';
        printf("SERVER -- Se leyeron %d bytes: %s\n", bytesRead, socketBuffer);
        serial_send(socketBuffer, bytesRead);
    }
    
    close(connFd);
    exit(EXIT_SUCCESS);
}
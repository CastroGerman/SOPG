#include <stdio.h>
#include <stdlib.h>
#include "SerialManager.h"
#include "mySerialService.h"
#include "tSerial.h"
#include "tServer.h"
#include <pthread.h>
#include <signal.h>
#include "sharedResources.h"

static pthread_t pthServer, pthSerial;

void signalBlock(void){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
}

void signalUnlock(void){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_UNBLOCK, &set, NULL);
}

void signalHandler(int sig){
    switch (sig){
        case SIGINT:
        case SIGTERM:
            pthread_cancel(pthServer);
            pthread_cancel(pthSerial);
            serverThread_closeResources();
            clientThread_closeResources();
            serialThread_closeResources();
            break;
        default:
            break;
    }
}

void registerSignals(void){
    struct sigaction sa;
    sa.sa_handler = &signalHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("Error al registrar SIGINT\n");
    }
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        printf("Error al registrar SIGTERM\n");
    }
}

int main(void){
    int retPthServer = -1, retPthSerial = -1;
    registerSignals();

    signalBlock();
    retPthServer = pthread_create(&pthServer, NULL, serverThread, NULL);
    if (retPthServer != 0) {
        printf("Error al crear thread de servidor TCP\n");
        exit(EXIT_FAILURE);
    }
    retPthSerial = pthread_create(&pthSerial, NULL, serialThread, NULL);
    if (retPthSerial != 0) {
        printf("Error al crear thread de comunicacion serial\n");
        exit(EXIT_FAILURE);
    }
    signalUnlock();
 
    pthread_join(pthServer, NULL);   
    pthread_join(pthSerial, NULL);        
	exit(EXIT_SUCCESS);
}

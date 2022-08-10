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
            pthread_kill(pthServerChild, SIGKILL);
            pthread_kill(pthServer, SIGKILL);
            pthread_kill(pthSerial, SIGKILL);
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
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int main(void){
    int retPthServer = -1, retPthSerial = -1;
    
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

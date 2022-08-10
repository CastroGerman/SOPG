#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define FIFO_NAME "myFifo"
#define STDIN_BUFFER_SIZE 30
#define MESSAGE_SIZE (STDIN_BUFFER_SIZE + 5)

static int32_t fdFifo = 0;

void userSignalHandler(int sig){
    if(0 == fdFifo) return;
    char msg[10];
    //printf("Entraste a ISR con %d\n", sig);
    switch (sig){
        case SIGUSR1:{
            strcpy(msg, "SIGN:1");
            break;
        }
        case SIGUSR2:{
            strcpy(msg, "SIGN:2");
            break;
        }
        default:{
            strcpy(msg, "SIGN:X");
            break;
        }
    }
    if (write(fdFifo, msg, strlen(msg)) == -1){
            printf("Error while trying to write named fifo\n");
            exit(1);
    }
}

void registerSignals(void){
    struct sigaction sa;
    sa.sa_handler = &userSignalHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
}

int main(void){
    
    registerSignals();

    char stdinBuff[STDIN_BUFFER_SIZE], msgBuff[MESSAGE_SIZE];
    (void)msgBuff;

    /* Create named fifo. -1 means already exists so 
    no action if already exists */
    if (mknod(FIFO_NAME, (S_IFIFO | 0666), 0) < -1){
        printf("Error creating named fifo");
        exit(1);
    }

    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for readers...\n");
	if ((fdFifo = open(FIFO_NAME, O_WRONLY)) < 0){
        printf("Error opening named fifo file: %d\n", fdFifo);
        exit(1);
    }
    printf("Got a reader!\n");

    while (1){
        /* Read stdin and write into named fifo.*/
        if (fgets(stdinBuff, STDIN_BUFFER_SIZE, stdin) != NULL){
            strcpy(msgBuff, "DATA:");
            strcat(msgBuff, stdinBuff);
            if (write(fdFifo, msgBuff, strlen(msgBuff) - 1) == -1){
            printf("Error while trying to write named fifo\n");
            exit(1);
            }
        }   
    }

    return 0;
}
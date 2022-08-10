#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define FIFO_NAME "myFifo"
#define STDIN_BUFFER_SIZE 30
#define MESSAGE_SIZE (STDIN_BUFFER_SIZE + 5)

#define LOG_FILE "logs.txt"
#define SIG_FILE "sigs.txt"

int writeFile(const char * fileName, const char * msg){
    FILE *fd = NULL;
    if ((fd = fopen(fileName, "a")) == NULL) return -1;
    fprintf(fd, "%s\n", msg);
    fclose(fd);    
    return 0;
}

int main(void){

    int32_t fdFifo = 0;
    int32_t bytesRead = 0;
    char stdinBuff[STDIN_BUFFER_SIZE], msgBuff[MESSAGE_SIZE];
    (void)msgBuff;

    /* Create named fifo. -1 means already exists so 
    no action if already exists */
    if (mknod(FIFO_NAME, (S_IFIFO | 0666), 0) < -1){
        printf("Error creating named fifo");
        exit(1);
    }

    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for writers...\n");
	if ((fdFifo = open(FIFO_NAME, O_RDONLY)) < 0){
        printf("Error opening named fifo file: %d\n", fdFifo);
        exit(1);
    }
    printf("Got a writer!\n");

    while (1){
        /* Read fifo.*/
        bytesRead = read(fdFifo, stdinBuff, STDIN_BUFFER_SIZE);
        if (bytesRead == -1){
            printf("Error while trying to write named fifo\n");
            printf("Goodbye!\n");
            exit(1);
        }else if (bytesRead == 0){
            printf("There is no writer left!\n");
            printf("Goodbye!\n");
            return 0;
        }else{
            stdinBuff[bytesRead] = '\0';
            printf("Read from fifo: %s\n", stdinBuff);
            if (strncmp(stdinBuff, "DATA", 4) == 0) {
                if (writeFile(LOG_FILE, stdinBuff) != 0) {
                    printf("Could not write to file.\n");
                }
            }
            if (strncmp(stdinBuff, "SIGN", 4) == 0) {
                if (writeFile(SIG_FILE, stdinBuff) != 0) {
                    printf("Could not write to file.\n");
                }
            }
        }
    }

    return 0;
}
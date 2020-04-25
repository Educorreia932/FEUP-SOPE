#include "flagsU.h"

#define MAX_THREADS 3
#define MAX_STR 50

void * thr_func(void * arg) {
    sleep(3);
    printf("%s\n", (char *) arg);
    return NULL;
}

int main(int argc, char * argv[]){
    // Check Flags
    flagsU* c = flagsU_constructor(); 
    
    parse_flagsU(argc, argv, c);

    if (argc != 4 || c->fifoname == NULL || c->nsecs == 0) {
        perror("Usage: U1 <-t nsecs> fifoname");
        exit(1);
    }

    //Open public FIFO
    int fd, timeout = 0;     

    do {
        fd = open(c->fifoname, O_WRONLY);

        if (fd == -1){
            timeout++;
            sleep(1);
        } 
            
    } while (fd == -1 && timeout < 3);

    if(timeout == 3){ //Took too long to open 
        perror("Failed to open FIFO");
        exit(1);
    } 

    //Thread creating
    pthread_t tid;
    char thrArg[MAX_STR];
    strcpy(thrArg, c->fifoname);

    if (pthread_create(&tid, NULL, thr_func, c->fifoname)){
        perror("Failed to create thread");
        exit(1);
    }

    close(fd);

    pthread_exit(NULL); //Thread continues running after the main thread ends

}
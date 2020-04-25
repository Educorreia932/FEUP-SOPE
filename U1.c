#include <time.h>
#include <sys/types.h>

#include "flagsU.h"

#define MAX_THREADS 3
#define MAX_STR 100

void * thr_func(void * arg) {
    char privateFIFO[MAX_STR], *tid;

    //Name of privateFIFO
    sprintf(privateFIFO, "/tmp/%d.", getpid());
    tid = (char *) malloc(sizeof(long unsigned int));
    sprintf(tid, "%lu", pthread_self());
    strcat(privateFIFO, tid);

    printf("%s\n", privateFIFO);
    return NULL;
}

int main(int argc, char * argv[]){
    
    //Begin Time count
    time_t begin = time(NULL);

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
    pthread_t tid[MAX_THREADS];
    
    char thrArg[MAX_STR];
    strcpy(thrArg, c->fifoname);

    int t = 0;
    while( (time(NULL)-begin) < c->nsecs && t < MAX_THREADS){
        if (pthread_create(&tid[t], NULL, thr_func, c->fifoname)){
            perror("Failed to create thread");
            exit(1);
        }

        if(usleep(100)){
            perror("Failed sleeping");
            exit(1);
        }
        
        t++;
    }
    
    for(int i = 0; i < t; i++){
        pthread_join(tid[i], NULL);
    }

    if(close(fd) == -1){
        perror("Failed closing fifo");
        exit(1);
    }

    return 0;

}
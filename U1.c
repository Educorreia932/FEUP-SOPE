#include <time.h>
#include <sys/types.h>
#include <semaphore.h>

#include "flagsU.h"
#include "log.h"

#define MAX_THREADS 10
#define MAX_STR 100

flagsU* flags;

sem_t mutex;

void * thr_func(void * arg) { 
    
    char msg[MAX_STR];
    //[ i, pid, tid, dur, pl] 
    sprintf(msg, "%d, %d, %lu, %d", * (int *)arg, getpid(), pthread_self(), rand() % 200 + 50);

    //Name of privateFIFO
    char privateFIFO[MAX_STR];
    sprintf(privateFIFO, "/tmp/%d.%lu", getpid(), pthread_self());
    
    int fd = open(privateFIFO, O_RDONLY | O_CREAT);

    if (fd == -1 ) {
        perror("Couldn't create private FIFO");
        pthread_exit(NULL);
    }

    //WAIT HERE

    

    enum Operation op = IWANT;
    print_log(msg, op);
    pthread_exit(NULL);
}

int main(int argc, char * argv[]){
    
    //Begin Time count
    time_t begin = time(NULL);

    // Check Flags
    flags = flagsU_constructor(); 
    
    parse_flagsU(argc, argv, flags);

    if (argc != 4 || flags->fifoname == NULL || flags->nsecs == 0) {
        perror("Usage: U1 <-t nsecs> fifoname");
        exit(1);
    }

    //Open public FIFO
    int fd, timeout = 0;     

    do {
        fd = open(flags->fifoname, O_WRONLY);

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
    int *thrArg, t = 0;
    
    srand(time(NULL));

    while( (time(NULL) - begin) < flags->nsecs && t < MAX_THREADS){
        
        thrArg = (int *) malloc(sizeof(t));
        *thrArg = t + 1; //request number

        if (pthread_create(&tid[t], NULL, thr_func, thrArg)){
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
#include "flagsQ.h"
#include <semaphore.h>

#include "utils.h"

static int public_fd;


void * handle_request(void * arg) { 
    
    char buffer[BUF_SIZE];

    read(public_fd, buffer, BUF_SIZE);

    printf("%s\n", buffer);

    pthread_exit(NULL);
}

int main(int argc, char * argv[]){

    time_t begin = time(NULL);

    // Check Flags
    flagsQ* c = flagsQ_constructor();
     
    parse_flagsQ(argc, argv, c);

    if (argc != 4 /*8 in Part2*/ || c->fifoname == NULL || c->nsecs == 0) {
        //perror("Usage: Q1 <-t nsecs> [-l nplaces] [-n nthreads] fifoname");
        perror("Usage: Q1 <-t nsecs> fifoname");
        exit(1);
    }

    //Create and open public FIFO
    if(mkfifo(c->fifoname, 0660) == -1){
        perror("Failed to create fifo");
        exit(1);
    }

    if ((public_fd = open(c->fifoname,O_RDONLY)) == -1) {
        perror("Couldn't open FIFO.\n");
        exit(1);
    }
    
    //Thread creating
    pthread_t tid[MAX_THREADS];
    int *thrArg, t = 0;
    
    srand(time(NULL));
    while( (time(NULL) - begin) < c->nsecs && t < MAX_THREADS){
        
        thrArg = (int *) malloc(sizeof(t));
        *thrArg = t + 1; //request number

        if (pthread_create(&tid[t], NULL, handle_request, thrArg)){
            perror("Failed to create thread");
            exit(1);
        }

        if(usleep(200)){
            perror("Failed sleeping");
            exit(1);
        }
        
        t++;
    }
    
    for(int i = 0; i < t; i++){
        pthread_join(tid[i], NULL);
    }
    
    //Close and Delete FIFO
    if(close(public_fd) == -1){
        perror("Failed closing fifo");
        exit(1);
    }

    if(unlink(c->fifoname) == -1){
        perror("Failed to delete FIFO");
        exit(1);
    }

    return 0;
}
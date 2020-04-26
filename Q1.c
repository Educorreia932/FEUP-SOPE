#include "flagsQ.h"
#include <semaphore.h>

#define BUF_SIZE 512

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
    int fd;

    if(mkfifo(c->fifoname, 0660) == -1){
        perror("Failed to create fifo");
        exit(1);
    }

    if ((fd = open(c->fifoname,O_RDONLY)) == -1) {
        perror("Couldn't open FIFO.\n");
        exit(1);
    }

    while ((time(NULL) - begin) < c->nsecs) {

        char request[BUF_SIZE];
        int n = 0;

        
        if ((n = read(fd,&request,1)) == -1){
            perror("Coudn´t read request");
            exit(1);
        }

        printf("%s\n", request);


    }

    //Close and Delete FIFO
    if(close(fd) == -1){
        perror("Failed closing fifo");
        exit(1);
    }

    if(unlink(c->fifoname) == -1){
        perror("Failed to delete FIFO");
        exit(1);
    }

    return 0;
}
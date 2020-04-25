#include "flagsQ.h"

int readline(int fd, char *str) {
    int n;
    
    do {
        n = read(fd,str,1);
    } while (n>0 && *str++ != '\0');
    
    return (n>0);
} 

int main(int argc, char * argv[]){
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
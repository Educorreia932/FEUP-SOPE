#include "flagsU.h"

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

    close(fd);

    return 0;
}
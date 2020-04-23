#include "flagsU.h"

int main(int argc, char * argv[]){
    // Check Flags
    flagsU* c = flagsU_constructor(); 
    
    parse_flagsU(argc, argv, c);

    if (argc != 4 || c->fifoname == NULL || c->nsecs == 0) {
        perror("Usage: U1 <-t nsecs> fifoname");
        exit(1);
    }

    int fd, messagelen;
    char message[100];     

    do {
        fd = open(c->fifoname, O_WRONLY);

        if (fd == -1) 
            sleep(1);
    } while (fd ==- 1); 

    for (int i=1; i<=3; i++) {
        sprintf(message,"Hello no. %d from process no. %d\n", i, getpid());
        messagelen=strlen(message)+1;
        write(fd,message,messagelen);
        sleep(1);
    } 

    close(fd);

    return 0;
}
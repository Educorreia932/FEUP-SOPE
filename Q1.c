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

    int fd;
    char str[100]; 

    mkfifo(c->fifoname, 0660);

    if ((fd = open(c->fifoname,O_RDONLY)) == -1) {
        perror("Couldn't open FIFO.\n");
        exit(1);
    }

    putchar('\n');
    while(readline(fd, str)) printf("%s",str);
    close(fd); 

    close(fd);

    return 0;
}
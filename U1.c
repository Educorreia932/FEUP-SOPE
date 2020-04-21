#include "flagsU.h"

int main(int argc, char * argv[]){

    //Check Flags
    flagsU* c = flagsU_constructor(); 
    parse_flagsU(argc, argv, c);

    if (argc != 4 || c->fifoname == NULL || c->nsecs == 0) {
        perror("Usage: U1 <-t nsecs> fifoname");
        exit(1);
    }

    print_flagsU(c);

    return 0;
}
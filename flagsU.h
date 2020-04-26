#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef struct {
    unsigned int nsecs;
    char * fifoname;
} flagsU;

void parse_flagsU(int argc, char* argv[], flagsU* c);

flagsU* flagsU_constructor();

void print_flagsU(flagsU* c);
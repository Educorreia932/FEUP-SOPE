#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned int nsecs;
    char * fifoname;
    //Parte 2
    //unsigned int nthreads;
    //unsigned int nplaces;
} flagsQ;

void parse_flagsQ(int argc, char* argv[], flagsQ* c);

flagsQ* flagsQ_constructor();

void print_flagsQ(flagsQ* c);
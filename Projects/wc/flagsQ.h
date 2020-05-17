#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct {
    unsigned int nsecs;
    char * fifoname;
    unsigned int nthreads;
    unsigned int nplaces;
} flagsQ;

void parse_flagsQ(int argc, char* argv[], flagsQ* c);

flagsQ* flagsQ_constructor();

void print_flagsQ(flagsQ* c);
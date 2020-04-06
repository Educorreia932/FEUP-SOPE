#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>    
#include <sys/types.h> 
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include "flags.h"

#define CURRENT 1
#define PARENT 2

int idgroup;

void handle_sigint(int sig);

bool isOriginal(char* envp[]);

int calculateSize(struct stat stat_buf, flags* c);


#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>    
#include <sys/types.h> 
#include <unistd.h>
#include "flags.h"

void handle_sigint(int sig) ;
bool check_folder(char* name);
bool isFather(char* envp[]);
int calculateSize(struct stat stat_buf, flags* c);


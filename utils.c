#include "utils.h"

// Handler for SIGINT, caused by 
// Ctrl-C at keyboard 
void handle_sigint(int sig) { 
    printf("Caught signal %d\n", sig); 
} 

bool isOriginal(char* envp[]) {
    if (getenv("envPid") == NULL){ 
        char env[50];
        sprintf(env, "%d", getpid());

        if (setenv("envPid", env, 0) == -1)
            exit(1);
        
        return true;
    }

    return false; 
} 

int calculateSize(struct stat stat_buf, flags* c) {
    int size;

    // If -b active
    if (c->bytes) 
        size = stat_buf.st_size;

    else
        size = stat_buf.st_blocks * 512;

    return size;
}
#include "utils.h"

bool check_folder(char* name) {
    const int path_length = strlen(name);

    bool current_folder = name[path_length - 1] == '.' && name[path_length - 2] == '/'; 
    bool parent_folder =  name[path_length - 1] == '.' && name[path_length - 2] == '.' && name[path_length - 3] == '/';

    if (current_folder)
        return CURRENT;

    else if (parent_folder)
        return PARENT;

    return 0;
}

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

int calculateSize(struct stat stat_buf, flags* c){
    int size;
    double aux;
    double multiplier = stat_buf.st_blocks != 0? 512.0 / (double)c->size : 1;

    if (c->bytes) //if -b active
        size = stat_buf.st_size;

    else{
        aux = stat_buf.st_blocks != 0? stat_buf.st_blocks * multiplier : 1;  

        //ceil(aux)
        if(aux - (int)aux > 0)
            size = (int)aux +1;
        else size = (int)aux;       
    }

    return size;
}
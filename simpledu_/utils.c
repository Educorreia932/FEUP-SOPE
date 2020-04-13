#include "utils.h"

// Handler for SIGINT, caused by 
// Ctrl-C at keyboard 
void handle_sigint(int sig) {
    kill(-idgroup, SIGSTOP);
    new_log(RECV_SIGNAL, "SIGINT", 0);
    new_log(SEND_SIGNAL, "SIGSTOP", idgroup);

    bool valid_answer = false;

    while(!valid_answer) {
        fflush(stdin);
        printf("\nAre you sure you want to cancel simpledu? (y/n) ");

        char answer;scanf(" %c", &answer);

        if(answer == 'y') {
            valid_answer = true;
            kill(-idgroup, SIGTERM);
            new_log(SEND_SIGNAL, "SIGTERM", idgroup);
            new_log(EXIT, NULL, 1);
            exit(1);
        }

        else if (answer == 'n') {
            valid_answer = true;
            printf("\n");
            kill(-idgroup, SIGCONT);
            new_log(SEND_SIGNAL, "SIGCONT", idgroup);
        }
    }
} 

bool isOriginal(char* envp[]) {
    if (getenv("envPid") == NULL){ 
        char env[50];
        sprintf(env, "%d", getpid());

        if (setenv("envPid", env, 0) == -1){
            new_log(EXIT, NULL, 1);
            exit(1);
        }
        return true;
    }
    return false; 
} 

void args_to_string(char **args, char *dest){

    char line[512] = "";
    
    for(int i = 0; args[i] != NULL; i++){
        strncat(line, args[i],512);
        strncat(line, " ",2);
    }
    
    strcpy(dest, line);
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

#include "log.h"

#define FILE_PERMISSION 0644

static char filename[50];

void create_log(char* evnp[]){
    char *log_file = getenv(LOG_ENVP);

    if (log_file == NULL) {
        if(setenv(LOG_ENVP, DEFAULT_LOG_FILENAME, 0))
            exit(1);

        log_file = DEFAULT_LOG_FILENAME;
    }
    
    strcpy(filename, log_file);
    
    int log_fd = open(DEFAULT_LOG_FILENAME, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC , FILE_PERMISSION);

    if (log_fd == -1) {
        perror(filename);
        exit(1);
    }
    close(log_fd);

    get_instance();
}

double get_instance(){
    struct timeval begin, end;
    double start, final, elapsed;  

    char aux[50];
    
    //Env has been set
    if (getenv(TIME_ENV) != NULL){ 
        //char *ptr;
        sprintf(aux, "%s", getenv(TIME_ENV));

        start = strtol(aux, NULL, 10);

    }

    // Original process (needs to set envp)
    else { 
        char t[50];
        gettimeofday(&begin, NULL);
        start = (begin.tv_sec * 1000000u + begin.tv_usec) / 1.e6;
        sprintf(t,"%f", start);

        if (setenv(TIME_ENV, t, 0) == -1){
            exit(1);
        }
    }

    gettimeofday(&end, NULL);
    final = (end.tv_sec * 1000000u + end.tv_usec) / 1.e6;
    elapsed = final - start;
    return elapsed*1000;
}

void new_log( action act, char *str, int num){
    
    int log_fd;

    log_fd = open(DEFAULT_LOG_FILENAME, O_WRONLY | O_CREAT | O_APPEND, FILE_PERMISSION);
    
    if (log_fd == -1) {
        perror(filename);
        exit(1);
    }

    char buff[MAX_BUFF_SIZE];

    int pid = getpid();
    double instance = get_instance();

    switch (act){
        case ENTRY:
            sprintf(buff, "%0.2f - %d - ENTRY - %s\n", instance, pid, str);
            break;
        case CREATE:
            sprintf(buff, "%0.2f - %u - CREATE - %s\n", instance, pid, str);
            break;
        case EXIT:
            sprintf(buff, "%0.2f - %u - EXIT - %d\n", instance, pid, num);
            break;
        case RECV_SIGNAL:
            sprintf(buff, "%0.2f - %d - RECV_SIGNAL - %s\n", instance,pid, str);
            break;
        case SEND_SIGNAL:
            sprintf(buff, "%0.2f - %d - SEND_SIGNAL - %s %d\n", instance, pid, str, num);
            break;
        case RECV_PIPE:
            sprintf(buff, "%0.2f - %d - RECV_PIPE - %d\n", instance, pid, num);
            break;
        case SEND_PIPE:
            sprintf(buff, "%0.2f - %d - SEND_PIPE - %d\n", instance, pid, num);
            break;

    }
    //printf("ali\n");
    int w = 0;
    if ((w = write(log_fd, buff, strlen(buff))) < 0){
        perror("write error");
        exit(1);
    }

    close(log_fd);

}
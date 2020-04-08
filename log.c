#include "log.h"


log_info *log_info_constructor() {

    log_info* l = (log_info*) malloc(sizeof(log_info));
    
    l->num = 0;
    int pid_dest = 0;

    return l;
}

void get_log_filename(char* evnp[], char *filename){

    char *log_file = getenv(LOG_ENVP);

    if (log_file == NULL) {

        if(setenv(LOG_ENVP, DEFAULT_LOG_FILENAME, 0)){
            exit(1);
        }
        log_file = DEFAULT_LOG_FILENAME;
    }
    
    strcpy(filename, log_file);
    
}

double get_instance(){
    
   struct timeval begin, end;
    double start, final, elapsed;  

    char aux[50];
    if (getenv(TIME_ENV) != NULL){ //env has been set
        //char *ptr;
        sprintf(aux, "%s", getenv(TIME_ENV));

        start = strtol(aux, NULL, 10);

    }
    else { //original process (needs to set envp)
        
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
    return elapsed;
}

void new_log( action act, int fd, log_info *l, char *str, bool original){
    int log_fd;

    if (original){
        log_fd = open(DEFAULT_LOG_FILENAME, O_WRONLY | O_CREAT | O_APPEND , 0644);
        get_instance();
    }
    else {
        log_fd = open(DEFAULT_LOG_FILENAME, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    if (log_fd == -1) {
    perror(DEFAULT_LOG_FILENAME);
    exit(1);
    }

    char buff[MAX_BUFF_SIZE];

    //end = time(NULL);
    //printf("aqui\n");
    int pid = getpid();
    double instance = get_instance();

    switch (act){

        case ENTRY:
            sprintf(buff, "%f - %d - ENTRY - %s\n", instance, pid, str);
            break;
        case CREATE:
            sprintf(buff, "%f - %u - CREATE - %s\n", instance, pid, str);
            break;
        case EXIT:
            sprintf(buff, "%f - %u - EXIT - %d\n", instance, pid, 0);
            break;
        case RECV_SIGNAL:
            sprintf(buff, "%f - %d - RECV_SIGNAL - %s\n", instance,pid, str);
            break;
        case SEND_SIGNAL:
            sprintf(buff, "%f - %d - SEND_SIGNAL - %s %d\n", instance, pid, str, l->pid_dest);
            break;
        case RECV_PIPE:
            sprintf(buff, "%f - %d - RECV_PIPE - %d\n", instance, pid, l->num);
            break;
        case SEND_PIPE:
            sprintf(buff, "%f - %d - SEND_PIPE - %d\n", instance, pid, l->num);
            break;

    }
    //printf("ali\n");
    int w = 0;
    if ((w = write(log_fd, buff, strlen(buff))) < 0){
        perror("write error");
        exit(1);
    }

}
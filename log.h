#pragma once

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>

#define LOG_ENVP "LOG_FILENAME"
#define DEFAULT_LOG_FILENAME "log.txt"
#define MAX_BUFF_SIZE 512

#define TIME_ENV "envTime"

typedef enum {CREATE, EXIT, RECV_SIGNAL, SEND_SIGNAL, RECV_PIPE, SEND_PIPE, ENTRY} action;


typedef struct {

    int pid_dest;
    int num; //for exit and signals

} log_info;

log_info *log_info_constructor();

void get_log_filename(char *envp[], char *filename);
void new_log( action act, int fd, log_info *info, char *str, bool original);
void begin_time();
double get_instance();
#pragma once

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>

#define LOG_ENVP "LOG_FILENAME"
#define DEFAULT_LOG_FILENAME "../log.txt"
#define MAX_BUFF_SIZE 512

#define TIME_ENV "envTime"

typedef enum {CREATE, EXIT, RECV_SIGNAL, SEND_SIGNAL, RECV_PIPE, SEND_PIPE, ENTRY} action;

void create_log(char *envp[]);
void new_log( action act, char *str, int num);
void begin_time();
double get_instance();
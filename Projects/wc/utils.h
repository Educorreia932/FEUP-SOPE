#pragma once

#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX_THREADS 10
#define NOT_SHARED 0
#define MAX_STR 100
#define BUF_SIZE 512

typedef struct {
    int i;
    pid_t pid;
    pthread_t tid;
    int dur;
    int pl;
} message_t;

message_t* message_constructor();

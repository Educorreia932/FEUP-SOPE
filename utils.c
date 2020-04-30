#include "utils.h"

#include <stdio.h>

message_t* message_constructor(int i) {
    srand(time(NULL));

    message_t* message = (message_t*) malloc(sizeof(message_t));

    message->i = i;
    message->pid = getpid();
    message->tid = pthread_self();
    message->dur = rand() % 200 + 50;
    message->pl = 0; // TODO: Second part

    return message;
}
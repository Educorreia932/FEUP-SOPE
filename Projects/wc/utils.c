#include "utils.h"

#include <stdio.h>

message_t* message_constructor(int i) {
    message_t* message = (message_t*) malloc(sizeof(message_t));

    message->i = i;
    message->pid = getpid();
    message->tid = pthread_self();
    message->dur = rand() % 15000 + 5000;
    message->pl = -1;

    return message;
}

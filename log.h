#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OP_LEN 6

enum Operation {
    IWANT,
    RECVD,
    ENTER,
    IAMIN,
    TIMUP,
    LATE, //2LATE
    CLOSD,
    FAILD,
    GAVUP
};

void print_log(char * msg, enum Operation op);
#include "log.h"

void print_log(char * msg, enum Operation op){

    char * aux;
    while( (aux = strstr(msg, ", ")) != NULL){ //Change ", " to " ; "
        strncpy(aux, " ; ", 3);
    }

    char operation[OP_LEN];
    switch(op){
        case IWANT:
            sprintf(operation, "IWANT");
            break;
        case RECVD:
            sprintf(operation, "RECVD");
            break;
        case ENTER:
            sprintf(operation, "ENTER");
            break;
        case IAMIN:
            sprintf(operation, "IAMIN");
            break;
        case TIMUP:
            sprintf(operation, "TIMUP");
            break;
        case LATE:
            sprintf(operation, "2LATE");
            break;
        case CLOSD:
            sprintf(operation, "CLOSD");
            break;
        case FAILD:
            sprintf(operation, "FAILD");
            break;
        case GAVUP:
            sprintf(operation, "GAVUP");
            break;
    }

    char log[200];
    sprintf(log, "%ld ; %s ; %s", time(NULL), msg, operation);

    printf("%s\n", log);

}
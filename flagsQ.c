#include "flagsQ.h"

flagsQ* flagsQ_constructor() {
    flagsQ* c = (flagsQ*) malloc(sizeof(flagsQ));

    c->nsecs = 0;
    c->fifoname = "";
    unsigned int nthreads = 0;
    unsigned int nplaces = 0;

    return c;
}

void parse_flagsQ(int argc, char* argv[], flagsQ* c) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], ""))
            continue;

        // fifoname
        if (argv[i][0] != '-' && (strcmp(argv[i - 1], "-t"))) 
            c->fifoname = argv[i];

        // nsecs
        else if (!strcmp(argv[i], "-t") && (i + 1 < argc)) { 
            char* argument = argv[i + 1];
            char* size;

            unsigned int val = strtol(argument, &size, 10);

            if ((size == argument) || (*size != '\0')) {
                free(c);
                perror("Size must be an integer\n");
                exit(1);
            }

            else
                c->nsecs = val;
            i++;
        }


        else if (!strcmp(argv[i], "-l") && (i + 1 < argc)) { 
            char* argument = argv[i + 1];
            char* size;

            unsigned int val = strtol(argument, &size, 10);

            if ((size == argument) || (*size != '\0')) {
                perror("Size must be an integer\n");
                exit(1);
            }

            else
                c->nplaces = val;
            i++;
        }

        else if (!strcmp(argv[i], "-n") && (i + 1 < argc)) { 
            char* argument = argv[i + 1];
            char* size;

            unsigned int val = strtol(argument, &size, 10);

            if ((size == argument) || (*size != '\0')) {
                perror("Size must be an integer\n");
                exit(1);
            }

            else
                c->nthreads = val;
                i++;
        } 

        else { // Checking for invalid arguments
            free(c);    
            printf("Q1: invalid option %s\n", argv[i]);
            exit(1);
        }
    }
}

void print_flagsQ(flagsQ* c) {
    printf("Fifoname: %s\n", c->fifoname);
    printf("Nsecs: %u\n", c->nsecs);
    printf("Nthreads: %u\n", c->nthreads);
    printf("Nplaces: %u\n", c->nplaces);
}
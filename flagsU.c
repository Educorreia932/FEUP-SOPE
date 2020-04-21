#include <flagsU.h>

flagsU* flagsU_constructor() {
    flagsU* c = (flagsU*) malloc(sizeof(flagsU));

    //c->nsecs = 0;
    //c->fifoname = "publicfifo";

    return c;
}

void parse_flagsU(int argc, char* argv[], flagsU* c) {
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
                perror("Size must be an integer\n");
                exit(1);
            }

            else
                c->nsecs = val;
            i++;
        }

        else { // Checking for invalid arguments
            printf("U1: invalid option %s\n", argv[i]);
            exit(1);
        }
    }
}

void print_flagsU(flagsU* c) {
    printf("Fifoname: %s\n", c->fifoname);
    printf("Nsecs: %u\n", c->nsecs);
}
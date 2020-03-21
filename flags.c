#include "flags.h"

void parse_flags(int argc, char* argv[], flags* c) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' && (strcmp(argv[i - 1], "-B")))
            c->path = argv[i];

        else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--all"))
            c->all = true;

        else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bytes"))
            c->bytes = true;

        else if (!strcmp(argv[i], "-B") && (i + 1 < argc)) {
            char* argument = argv[i + 1];
            char* size;

            unsigned int val = strtol(argument, &size, 10);

            if ((size == argument) || (*size != '\0')) {
                perror("Size must be an integer\n");
                exit(1);
            }

            else
                c->size = atoi(argument);
        }

        else if (!strcmp(argv[i], "-L") || !strcmp(argv[i], "--dereference"))
            c->dereference = true;

        else if (!strcmp(argv[i], "-S") || !strcmp(argv[i], "--separate-dirs"))
            c->separate_dirs = true;

        else if (strstr(argv[i], "--max-depth=") != NULL) {
            char* max_depth;
            char* token = strtok(argv[i], "=");
            token = strtok(NULL, "=");

            unsigned int val = strtol(token, &max_depth, 10);

            if ((max_depth == token) || (*max_depth != '\0')) {
                perror("Max depth must be an integer\n");
                exit(1);
            }

            else
                c->max_depth = atoi(token);
        }
    }
}

void print_flags(flags* c) {
    printf("Path: %s\n", c->path);
    printf("All: %s\n", c->all? "YES" : "NO");
    printf("Bytes: %s\n", c->bytes? "YES" : "NO");
    printf("Size: %u\n", c->size);
    printf("Dereference: %s\n", c->dereference? "YES" : "NO");
    printf("Separate directories: %s\n", c->separate_dirs? "YES" : "NO");
    printf("Max depth: %u\n", c->max_depth);
}
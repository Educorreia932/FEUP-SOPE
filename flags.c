#include "flags.h"


flags* flags_constructor() {
    flags* c = (flags*) malloc(sizeof(flags));

    c->path = ".";
    c->all = false;
    c->bytes = false;
    c->size = DEFAULT_BLOCK_SIZE;
    c->count_links = false;
    c->dereference = false;
    c->separate_dirs = false;
    c->max_depth = INT_MAX;

    return c;
}

void parse_flags(int argc, char* argv[], flags* c) {

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' && (strcmp(argv[i - 1], "-B")))
            c->path = argv[i];

        else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--all")) //all files
            c->all = true;

        else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bytes")) //real number of bytes
            c->bytes = true;

        else if (!strcmp(argv[i], "-B") && (i + 1 < argc)) { //block size (bytes)
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
        else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--count-links")){ //count size many times
            c->count_links = true;

        }
        else if (!strcmp(argv[i], "-L") || !strcmp(argv[i], "--dereference")) //symbolic links
            c->dereference = true;

        else if (!strcmp(argv[i], "-S") || !strcmp(argv[i], "--separate-dirs")) //exclude sub directory info
            c->separate_dirs = true;

        else if (strstr(argv[i], "--max-depth=") != NULL) { //directory max depth
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


//-----DEBUG
void print_flags(flags* c) {
    printf("Path: %s\n", c->path);
    printf("All: %s\n", c->all? "YES" : "NO");
    printf("Bytes: %s\n", c->bytes? "YES" : "NO");
    printf("Size: %u\n", c->size);
    printf("Count links: %s\n", c->count_links? "YES" : "NO");
    printf("Dereference: %s\n", c->dereference? "YES" : "NO");
    printf("Separate directories: %s\n", c->separate_dirs? "YES" : "NO");
    printf("Max depth: %u\n", c->max_depth);
}
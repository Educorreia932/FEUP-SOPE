#include "flags.h"

flags* flags_constructor() {
    flags* c = (flags*) malloc(sizeof(flags));

    c->path = ".";
    c->all = false;
    c->bytes = false;
    c->size = DEFAULT_BLOCK_SIZE;
    c->dereference = false;
    c->separate_dirs = false;
    c->max_depth = INT_MAX;

    return c;
}

void parse_flags(int argc, char* argv[], flags* c) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], ""))
            continue;

        // Path
        if (argv[i][0] != '-' && (strcmp(argv[i - 1], "-B"))) 
            c->path = argv[i];

        // Include files
        else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--all")) 
            c->all = true;

        // Real number of bytes
        else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--bytes")) {
            c->bytes = true;
            c->size = 1;
        }

        // Block size (bytes)
        else if ((!strcmp(argv[i], "-B")) && (i + 1 < argc)) { 
            char* argument = argv[i + 1];
            char* size;

            unsigned int val = strtol(argument, &size, 10);

            if ((size == argument) || (*size != '\0')) {
                perror("Size must be an integer\n");
                new_log(EXIT, NULL, 1);
                exit(1);
            }

            else
                c->size = val;

            i++;
        }

        else if (strstr(argv[i], "--block-size=") != NULL) { 
            char* block_size;
            char* token = strtok(argv[i], "=");
            token = strtok(NULL, "=");

            unsigned int val = strtol(token, &block_size, 10);

            if ((block_size == token) || (*block_size != '\0')) {
                perror("Block size must be an integer\n");
                new_log(EXIT, NULL, 1);
                exit(1);
            }

            else
                c->size = atoi(token);
        }

        // Symbolic links
        else if (!strcmp(argv[i], "-L") || !strcmp(argv[i], "--dereference")) 
            c->dereference = true;

        // Exclude sub directory info
        else if (!strcmp(argv[i], "-S") || !strcmp(argv[i], "--separate-dirs")) 
            c->separate_dirs = true;

        // Directory max depth
        else if (strstr(argv[i], "--max-depth=") != NULL) { 
            char* max_depth;
            char* token = strtok(argv[i], "=");
            token = strtok(NULL, "=");

            unsigned int val = strtol(token, &max_depth, 10);

            if ((max_depth == token) || (*max_depth != '\0')) {
                perror("Max depth must be an integer\n");
                new_log(EXIT, NULL, 1);
                exit(1);
            }

            else
                c->max_depth = atoi(token);
        }

        // Checking for invalid arguments
        else if (strcmp(argv[i], "-l") && strcmp(argv[i], "--count-links")){ 
            printf("simpledu: invalid option %s\n", argv[i]);
            new_log(EXIT, NULL, 1);
            exit(1);
        }
    }
}

void create_child_command(flags *c, char *name, char **dest){
    char log_line[512] = "";

    dest[1] = name;

    int cnt = 1;
    if (c->all) { dest[++cnt] = "-a"; }
    if (c->bytes) { dest[++cnt] = "-b"; }
    if (c->separate_dirs) { dest[++cnt] = "-S"; }

    if (c->max_depth != INT_MAX) {
        dest[++cnt] = calloc(50, sizeof(char));
        sprintf(dest[cnt], "--max-depth=%d", c->max_depth - 1);
    }

    if (c->size != DEFAULT_BLOCK_SIZE) {
        dest[++cnt] = "-B";
        dest[++cnt] = calloc(25, sizeof(char));
        sprintf(dest[cnt], "%u", c->size);
    }

    if (c->separate_dirs) 
        dest[++cnt] = "-S"; 
        
    if (c->dereference)
        dest[++cnt] = "-L";
    
    dest[++cnt] = NULL;

    //converting args to string
    for ( int i = 0; dest[i] != NULL; i++) {
        strncat(log_line, dest[i],512);
        strncat(log_line, " ",2);
    } 

    new_log(CREATE, log_line, 0);
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
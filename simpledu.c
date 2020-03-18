#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h> 
#include <stdbool.h>
#include <limits.h>

typedef struct {
    char* path;
    bool all;
    bool bytes;
    unsigned int size;
    bool dereference;
    bool separate_dirs;
    unsigned int max_depth;
} commands;

void parse_commands(int argc, char* argv[], commands* c) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' && (strcmp(argv[i - 1], "-B")))
            c->path = argv[i];

        else if (!strcmp(argv[i], "-a"))
            c->all = true;

        else if (!strcmp(argv[i], "-b"))
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

        else if (!strcmp(argv[i], "-L"))
            c->dereference = true;

        else if (!strcmp(argv[i], "-S"))
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

void print_commands(commands* c) {
    printf("Path: %s\n", c->path);
    printf("All: %s\n", c->all? "YES" : "NO");
    printf("Bytes: %s\n", c->bytes? "YES" : "NO");
    printf("Size: %u\n", c->size);
    printf("Dereference: %s\n", c->dereference? "YES" : "NO");
    printf("Separate directories: %s\n", c->separate_dirs? "YES" : "NO");
    printf("Max depth: %u\n", c->max_depth);
}

int main(int argc, char* argv[], char* envp[]) {
    DIR *dirp;
    struct dirent *direntp;
    struct stat stat_buf;
    char *str;
    char name[200]; 
    commands* c = (commands*) malloc(sizeof(commands));

    c->path = "./";
    c->all = false;
    c->bytes = false;
    c->size = 1024;
    c->dereference = false;
    c->separate_dirs = false;
    c->max_depth = UINT_MAX;

    if (argc < 2 || argc > 9) {
        perror("Usage: simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]");
        exit(1);
    }

    parse_commands(argc, argv, c);

    // print_commands(c);

    if ((dirp = opendir(c->path)) == NULL) {
        perror(c->path);
        exit(3);
    } 
    
    while ((direntp = readdir( dirp)) != NULL) {
        sprintf(name, "%s/%s", c->path, direntp->d_name);
        
        if (lstat(name, &stat_buf) == -1) {
            perror("lstat ERROR");
            exit(3);
        }

        int size;
        double multiplier = stat_buf.st_blocks != 0? 512.0 / c->size : 0;

        if (c->bytes)
            size = stat_buf.st_size;

        else
            size = stat_buf.st_blocks != 0? stat_buf.st_blocks * multiplier : multiplier;           

        // if (S_ISREG(stat_buf.st_mode)) 
        //     str = "regular";
        
        // else if (S_ISDIR(stat_buf.st_mode))
        //     str = "directory";

        //     // if (pid_t pid = fork())
        
        // else 
        //     str = "other";
        
        printf("%-20u ./%s\n", size, direntp->d_name);
    }

    closedir(dirp); 

    exit(0);
}
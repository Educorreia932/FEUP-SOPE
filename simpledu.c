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
    unsigned int sizes;
    bool dereference;
    bool separate_dirs;
    unsigned int max_depth;
} commands;

void parse_commands(int argc, char* argv[], commands* c) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-')
            c->path = argv[i];

        else if (!strcmp(argv[i], "-a"))
            c->all = true;

        else if (!strcmp(argv[i], "-b"))
            c->bytes = true;

        else if (!strcmp(argv[i], "-L"))
            c->dereference = true;

        else if (!strcmp(argv[i], "-S"))
            c->separate_dirs = true;

        else if (!strcmp(argv[i], "-S") && (i + 1 <= argc) && (is_digit(argv[i + 1])))
            c->max_depth = atoi(argv[i + 1]);
        }
    }
}

void print_commands(commands* c) {
    printf("Path: %s\n", c->path);
    printf("All: %s\n", c->all? "YES" : "NO");
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
    c->dereference = false;
    c->separate_dirs = false;
    c->max_depth = UINT_MAX;

    if (argc < 2 || argc > 9) {
        perror("Usage: simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]");
        exit(1);
    }

    if ((dirp = opendir(c->path)) == NULL) {
        perror(argv[1]);
        exit(2);
    } 

    parse_commands(argc, argv, c);

    print_commands(c);

    // while ((direntp = readdir( dirp)) != NULL) {
    //     sprintf(name,"%s/%s",argv[1],direntp->d_name); // <----- NOTAR
        
    //     // alternativa a chdir(); ex: anterior
    //     if (lstat(name, &stat_buf)==-1) {
    //         perror("lstat ERROR");
    //         exit(3);
    //     }

    //     // printf("%10d - ",(int) stat_buf.st_ino);
    //     if (S_ISREG(stat_buf.st_mode)) 
    //         str = "regular";
        
    //     else if (S_ISDIR(stat_buf.st_mode))
    //         str = "directory";
        
    //     else 
    //         str = "other";
        
    //     printf("%-25s - %s\n", direntp->d_name, str);
    // }

    closedir(dirp); 

    exit(0);
}
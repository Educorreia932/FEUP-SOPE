#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <errno.h> 
#include <stdbool.h>
#include <limits.h>

#include "flags.h"

int main(int argc, char* argv[], char* envp[]) {
    DIR *dirp;
    struct dirent *direntp;
    struct stat stat_buf;
    char *str;
    char name[300]; 
    flags* c = (flags*) malloc(sizeof(flags));

    c->path = ".";
    c->all = false;
    c->bytes = false;
    c->size = DEFAULT_BLOCK_SIZE;
    c->dereference = false;
    c->separate_dirs = false;
    c->max_depth = UINT_MAX;

    if (argc > 9) {
        perror("Usage: simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]");
        exit(1);
    }

    parse_flags(argc, argv, c);

    if ((dirp = opendir(c->path)) == NULL) {
        perror(c->path);
        exit(2);
    } 
    
    while ((direntp = readdir(dirp)) != NULL) {
        // Format path for each directory/file
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

        if (S_ISLNK(stat_buf.st_mode))
            break;

        // File
        if (S_ISREG(stat_buf.st_mode)) {
            printf("%-7u %s\n", size, name);
        }
        
        // Directory
        else if (S_ISDIR(stat_buf.st_mode)) {
            pid_t pid = 1;

            if (name[strlen(name) - 1] == '.')
                continue;

            else
                pid = fork();

            int status;

            if (pid == 0) {
                char* argv_[3] = {"simpledu", name, NULL};

                if (execve("simpledu", argv_, envp))
                    printf("Error in exec %s\n", name);
            }

            else {
                wait(&status);
                printf("%-7u %s\n", size, name);
            }
        }
    }

    closedir(dirp); 

    exit(0);
}
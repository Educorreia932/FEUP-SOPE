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
#include "utils.h"

#define READ 0
#define WRITE 1

int main(int argc, char* argv[], char* envp[]) {
    DIR *dirp;
    struct dirent *direntp;
    struct stat stat_buf;
    char name[300]; 
    flags* c = flags_constructor();
    int folder_size = 0;
    int fd[2];

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
            char str[200];

            sprintf(str, "%-7u %s\n", size, name);

            if (c->max_depth > 0 && c->all)
                write(STDOUT_FILENO, str, strlen(str));

            folder_size += size;
        }
        
        // Directory
        else if (S_ISDIR(stat_buf.st_mode)) {

            if (pipe(fd) < 0) 
                perror("Pipe error %s\n");  

            pid_t pid = -1;

            if (check_folder(name))
                continue;

            else
                pid = fork();

            // Parent
            if (pid > 0) {
                int child_size;

                close(fd[WRITE]);

                read(fd[READ], &child_size, sizeof(int));

                folder_size += child_size;

                if (c->max_depth > 0) {
                    char str[200];

                    sprintf(str, "%-7u %s\n", child_size, name);
                    
                    write(STDOUT_FILENO, str, strlen(str));
                }
            }

            // Child
            else if (pid == 0) {
                close(fd[READ]);
                dup2(fd[WRITE], 999);
                close(fd[WRITE]);

                char max_depth[50];
                sprintf(max_depth, "--max-depth=%u", c->max_depth - 1);

                char* argv_[5] = {"simpledu", name, max_depth, c->all? "-a" : NULL, NULL};

                if (execve("simpledu", argv_, envp) == -1)
                    perror("Error in exec\n");
            }

            // Error
            else {
                perror("Error in fork\n");
                exit(3);
            }
        }
    }

    char tmp[27];
    sprintf(tmp, "%d", getpid());

    write(999, &folder_size, sizeof(int));

    wait(NULL);
    
    closedir(dirp); 
    close(fd[READ]);
    close(fd[WRITE]);
    exit(0);
}
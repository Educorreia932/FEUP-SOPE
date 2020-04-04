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
#define envPid  "envPid"

int main(int argc, char* argv[], char* envp[]) {
    DIR *dirp;
    struct dirent *direntp;
    struct stat stat_buf;
    char name[300]; 
    flags* c = flags_constructor();
    int folder_size = 0;
    int fd[2];
    char size_currentDir[50];
    bool iamfather = false;



    if (argc > 9) {
        perror("Usage: simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]");
        exit(1);
    }

    parse_flags(argc, argv, c);
    
    if ((dirp = opendir(c->path)) == NULL) {
        perror(c->path);
        exit(1);
    } 

    if(getenv("envPid") == NULL){ //Sou pai
        char env[50];
        sprintf(env, "%d", getpid());
        if(setenv("envPid", env, 0) == -1)
            exit(1);
        //printf("ENTREI\n");
        iamfather = true;
    }
    /*else{
        printf("NAO SOU FILHO \n ");
    }*/


    while ((direntp = readdir(dirp)) != NULL) {
        // Format path for each directory/file
        sprintf(name, "%s/%s", c->path, direntp->d_name);

        if(c->dereference){
            if (stat(name, &stat_buf) == -1) {
                perror("lstat ERROR");
                exit(1);
            }
        }
        else{
            if (lstat(name, &stat_buf) == -1) {
                perror("lstat ERROR");
                exit(1);
            }
        }

        
        int size;
        double aux;
        double multiplier = stat_buf.st_blocks != 0? 512.0 / (double)c->size : 1;

        if (c->bytes)
            size = stat_buf.st_size;

        else{
            aux = stat_buf.st_blocks != 0? stat_buf.st_blocks * multiplier : 1;  

            if(aux - (int)aux > 0)
                size = (int)aux +1;
            else size = (int)aux;       
        }
            

        //if (S_ISLNK(stat_buf.st_mode))
            //break;

        // File
        if (S_ISREG(stat_buf.st_mode) || (c->dereference && S_ISLNK(stat_buf.st_mode))) {
            char str[200];

            sprintf(str, "%-7u %s\n", size, name);

            if (c->max_depth > 0 && c->all)
                write(STDOUT_FILENO, str, strlen(str));

            folder_size += size;
        }
        
        // Directory
        else if (S_ISDIR(stat_buf.st_mode) || (c->dereference && S_ISLNK(stat_buf.st_mode))) {
            if (pipe(fd) < 0) 
                perror("Pipe error %s\n");  

            pid_t pid = -1;

            if (check_folder(name))
                continue;
            else if (name[strlen(name) - 1] == '.' && name[strlen(name) - 2] == '/'){
                int size;
                double aux;
                double multiplier = stat_buf.st_blocks != 0? 512.0 / (double)c->size : 1;

                if (c->bytes)
                    size = stat_buf.st_size;

                else{
                    aux = stat_buf.st_blocks != 0? stat_buf.st_blocks * multiplier : 1;  

                    if(aux - (int)aux > 0)
                        size = (int)aux +1;
                    else size = (int)aux;       
                }


                folder_size += size;


            }

            else{
                pid = fork();

                // Parent
                if (pid > 0) {
                    int child_size;

                    close(fd[WRITE]);
                    
                    read(fd[READ], &child_size, sizeof(int));

                    if(!c->separate_dirs ) //-S
                    {
                        folder_size += child_size;

                    }    

                
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
                    char B[50];
                    sprintf(B, "%u", c->size);
                    char* argv_[9] = {"simpledu", name, max_depth, c->all? "-a" : "", c->bytes? "-b" : "", c->separate_dirs? "-S" : "", "-B", B ,NULL};

                    if (execv("simpledu", argv_) == -1)
                        perror("Error in exec\n");
                }

                // Error
                else {
                    perror("Error in fork\n");
                    exit(3);
                }
            }
                
        }
    }

    char tmp[27];
    sprintf(tmp, "%d", getpid());
    write(999, &folder_size, sizeof(int));

    wait(NULL);
    
    if(iamfather){
        sprintf(size_currentDir, "%-7u .\n", folder_size);
        write(STDOUT_FILENO, size_currentDir, strlen(size_currentDir));
    }

    closedir(dirp); 
    close(fd[READ]);
    close(fd[WRITE]);
    exit(0);
}
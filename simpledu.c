#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h> 
 #include <errno.h> 
#include <limits.h>
#include <signal.h>

#include "utils.h"

#define READ 0
#define WRITE 1


int main(int argc, char* argv[], char* envp[]) {
    char name[300]; 
    int folder_size = 0;
    int fd[2];
    char size_currentDir[50];

    //Signal Handler
    signal(SIGINT, handle_sigint); 

    //Check Flags
    flags* c = flags_constructor();

    if (argc > 9) {
        perror("Usage: simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]");
        exit(1);
    }

    parse_flags(argc, argv, c);

    // Checks if this one is the original process
    bool original = isOriginal(envp);

    // Open directory
    DIR *dirp;
    if ((dirp = opendir(c->path)) == NULL) {
        perror(c->path);
        exit(1);
    } 

    // Read contents of current directory
    struct dirent *direntp;
    while ((direntp = readdir(dirp)) != NULL) {

        // Format path for each directory/file
        sprintf(name, "%s/%s", c->path, direntp->d_name);

        //Info
        struct stat stat_buf;
        if(c->dereference){ //Stat if -S active

            if (stat(name, &stat_buf) == -1) {
                perror("lstat ERROR");
                exit(1);
            }
        }
        else{ //Lstat if -S not active

            if (lstat(name, &stat_buf) == -1) {
                perror("lstat ERROR");
                exit(1);
            }
        }

        // Calculates size
        int size = calculateSize(stat_buf, c);

        // FILE (or symb link if -S)
        if (S_ISREG(stat_buf.st_mode) || (c->dereference && S_ISLNK(stat_buf.st_mode))) {
            
            // Format print with size
            if (c->max_depth > 0 && c->all){
                char str[200];
                sprintf(str, "%-7u %s\n", size, name);
                write(STDOUT_FILENO, str, strlen(str));
            }

            folder_size += size;
        }
        
        // Directory
        else if (S_ISDIR(stat_buf.st_mode) || (c->dereference && S_ISLNK(stat_buf.st_mode))) {
            if (pipe(fd) < 0) 
                perror("Pipe error %s\n");  

            pid_t pid = -1;

            int folder_type = check_folder(name);

            if (folder_type == PARENT)
                continue;

            else if (folder_type == CURRENT) {
                int size;
                double aux;
                double multiplier = stat_buf.st_blocks != 0? 512.0 / (double)c->size : 1;

                if (c->bytes)
                    size = stat_buf.st_size;

                else {
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

                    char *argv_[50];
                    create_child_command(c, name, argv_);

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
    
    if (original) {
        sprintf(size_currentDir, "%-7u %s\n", folder_size, c->path);

        write(STDOUT_FILENO, size_currentDir, strlen(size_currentDir));
    }

    closedir(dirp); 
    close(fd[READ]);
    close(fd[WRITE]);
    exit(0);
}
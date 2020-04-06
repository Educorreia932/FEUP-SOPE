#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <errno.h> 
#include <limits.h>
#include <signal.h>

#include "utils.h"
#include "queue.h"

#define READ 0
#define WRITE 1

int main(int argc, char* argv[], char* envp[]) {
    char name[300]; 
    int folder_size = 0;
    int fd[2];
    char size_currentDir[50];
    Queue_t* file_descriptors = new_queue();

    idgroup = getpgid(getpid());

    //Signal Handler
    if(isOriginal)
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
        if (!strcmp(direntp->d_name, "..")) 
            continue;

        // Format path for each directory/file
        sprintf(name, "%s/%s", c->path, direntp->d_name);

        // Info
        struct stat stat_buf;

        // Stat if -S active
        if (c->dereference && stat(name, &stat_buf) == -1){ 
            perror("lstat ERROR");
            exit(1);
        }

        // Lstat if -S not active
        else if (lstat(name, &stat_buf) == -1) {
            perror("lstat ERROR");
            exit(1);
        }

        // Calculates size
        int size = calculateSize(stat_buf, c);

        // FILE (or symb link if -S)
        if (S_ISREG(stat_buf.st_mode) || (c->dereference && S_ISLNK(stat_buf.st_mode))) {
            // Format print with size
            if (c->max_depth > 0 && c->all){
                char str[400];

                if (!c->bytes)
                    size = (size + c->size - 1) / c->size;

                sprintf(str, "%-7u %s\n", size, name);
                write(STDOUT_FILENO, str, strlen(str));
            }

            folder_size += size;
        }
        
        // Directory
        else if (S_ISDIR(stat_buf.st_mode) || (c->dereference && S_ISLNK(stat_buf.st_mode))) {
            if (pipe(fd) < 0) 
                perror("Pipe error %s\n");  

            if (!strcmp(direntp->d_name, ".")){
                calculateSize(stat_buf, c);

                folder_size += size;
            }

            else {
                pid_t pid = -1;
                pid = fork();

                //Checks if firstChild
                bool first_child = false;
                
                if(original)
                    first_child = true;   

                // Parent
                if (pid > 0) {
                    int child_size;

                    close(fd[WRITE]);
                    
                    if(original)
                    {
                        char ready;
                        while(read(fd[READ], &ready, sizeof(char)) == 0);
                        idgroup = pid;
                    }

                    while(read(fd[READ], &child_size, sizeof(int)) == 0);

                    if (!c->separate_dirs ) //-S
                        folder_size += child_size;
                
                    if (c->max_depth > 0) {
                        char str[400];

                        if (!c->bytes)
                            child_size = (child_size + c->size - 1) / c->size;

                        sprintf(str, "%-7u %s\n", child_size, name);
                        
                        write(STDOUT_FILENO, str, strlen(str));
                    }
                }

                // Child
                else if (pid == 0) {
                    close(fd[READ]);
                    dup2(fd[WRITE], 999);
                    close(fd[WRITE]);

                    if(first_child)
                    {
                        char ready = 'y';
                        setpgid(0, getpid());
                        write(999, &ready, sizeof(char)); //Tells father group is set
                    }
                    else
                        setpgid(pid, idgroup);
                    
                    

                    char *argv_[50];
                    create_child_command(c, name, argv_);

                    if (execv("simpledu", argv_) == -1)
                        perror("Error in exec\n");
                }

                // Error
                else {
                    perror("Error in fork\n");
                    exit(1);
                }
            }
                
        }
    }


    if (original) {
        if (!c->bytes)
            folder_size = (folder_size + c->size - 1) / c->size;

        sprintf(size_currentDir, "%-7u %s\n", folder_size, c->path);

        write(STDOUT_FILENO, size_currentDir, strlen(size_currentDir));
    }

    else {
        char tmp[27];
        sprintf(tmp, "%d", getpid());
        write(999, &folder_size, sizeof(int));
    }

    closedir(dirp); 
    close(fd[READ]);
    close(fd[WRITE]);

    exit(0);
}

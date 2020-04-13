#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <errno.h> 
#include <limits.h>
#include <signal.h>
#include <fcntl.h>

#include "utils.h"
#include "queue.h"

#define READ 0
#define WRITE 1

int main(int argc, char* argv[], char* envp[]) {

    char name[300]; 
    int folder_size = 0;
    int fd[2];
    char size_currentDir[50];
    int numChildren = 0;

    //Group ID
    idgroup = getpgid(getpid());

    // Checks if this one is the original process
    bool original = isOriginal(envp);

    //Signal Handler
    if(original) { 
        signal(SIGINT, handle_sigint); 
        create_log(envp);
    }

    //Check Flags
    flags* c = flags_constructor();

    if (argc > 9) {
        perror("Usage: simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]");
        new_log(EXIT, NULL, 1);
        exit(1);
    }

    parse_flags(argc, argv, c);
    
    //LOG Create original
    if(original){
        char args[512];
        args_to_string(argv, args);
        new_log(CREATE, args, 0);
    }

    // Open directory
    DIR *dirp;

    if ((dirp = opendir(c->path)) == NULL) {
        new_log(EXIT, NULL, 1);
        perror(c->path);
        exit(1);
    } 

    // Read contents of current directory
    struct dirent *direntp;

    //QUEUE of fds
    Queue_t *qfds = new_queue();

    while ((direntp = readdir(dirp)) != NULL) {
        if (!strcmp(direntp->d_name, "..")) 
            continue;

        // Format path for each directory/file
        sprintf(name, "%s/%s", c->path, direntp->d_name);

        // Info
        struct stat stat_buf;

        // Stat if -L active
        if (c->dereference) {
            if (stat(name, &stat_buf) == -1){
                new_log(EXIT, NULL, 1); 
                perror("stat ERROR");
                exit(1);
            }
        }

        // Lstat if -L not active
        else {
            if (lstat(name, &stat_buf) == -1) {
            new_log(EXIT, NULL, 1);
            perror("lstat ERROR");
            exit(1);}
        }

        // Calculates size
        int size = calculateSize(stat_buf, c);

        // FILE (or symb link if -S)
        if (S_ISREG(stat_buf.st_mode) || (c->dereference && S_ISLNK(stat_buf.st_mode))) {
            // Format print with size
            folder_size += size;

            if (c->max_depth > 0 && c->all){
                char str[400];

                if (!c->bytes)
                    size = (size + c->size - 1) / c->size;

                else
                    size /= c->size;

                sprintf(str, "%-7u %s\n", size, name);
                write(STDOUT_FILENO, str, strlen(str));
            }
        }
        
        // Directory
        else if (S_ISDIR(stat_buf.st_mode) ) {
            if (pipe(fd) < 0){
                perror("Pipe error %s\n"); 
                new_log(EXIT, NULL, 1);
                exit(1);
            }  

            if (!strcmp(direntp->d_name, "."))
                folder_size += size;

            else {
                pid_t pid = -1;
                numChildren++;
                pid = fork();

                // Parent
                if (pid > 0) {
                    int child_size;

                    if (close(fd[WRITE]) == -1){
                        new_log(EXIT, NULL, 1);
                        exit(1);
                    }    

                    queue_push_back(qfds, fd[READ]);

                    if(original && (numChildren == 1)) {
                        char ready;
                        while(read(fd[READ], &ready, sizeof(char)) == 0);
                        new_log(RECV_PIPE, NULL, ready);
                        idgroup = pid;
                    }
                }

                // Child
                else if (pid == 0) {                    
                    if(close(fd[READ]) == -1){
                        new_log(EXIT, NULL, 1);
                        exit(1);
                    }

                    dup2(fd[WRITE], 999);

                    if (close(fd[WRITE]) == -1){
                        new_log(EXIT, NULL, 1);
                        exit(1);
                    }

                    if (original && numChildren == 1) {
                        char ready = 'y';
                        setpgid(0, getpid());
                        write(999, &ready, sizeof(char)); //Tells father group is set
                        new_log(SEND_PIPE, NULL, ready);
                    }

                    else
                        setpgid(pid, idgroup);

                    char *argv_[50];
                    argv_[0] = argv[0];
                    create_child_command(c, name, argv_);
        
                    if (execv(argv[0], argv_) == -1){
                        perror("Error in exec\n");
                        new_log(EXIT, NULL, 1);
                        exit(1);
                    }
                }

                // Error
                else {
                    perror("Error in fork\n");
                    new_log(EXIT, NULL, 1);
                    exit(1);
                }
            }
        }
    }

    pid_t wpid;
    int status;
    int cSize = 0;

    while(!queue_is_empty(qfds)){
        wait(NULL);

        read(queue_pop(qfds), &cSize, sizeof(int));
        new_log(RECV_PIPE, NULL, cSize);
        
        if (!c->separate_dirs ) //-S
            folder_size += cSize;
    }
    
    int auxF = (folder_size + c->size - 1) / c->size;

    sprintf(size_currentDir, "%-7u %s\n", auxF, c->path);

    if (c->max_depth >= 0)
        write(STDOUT_FILENO, size_currentDir, strlen(size_currentDir));

    write(999, &folder_size, sizeof(int));
    new_log(SEND_PIPE, NULL, folder_size);

    if(closedir(dirp) == -1){
        new_log(EXIT, NULL, 1);
        exit(1);
    } 
    
    if(close(fd[READ]) == -1){
        new_log(EXIT, NULL, 1);
        exit(1);
    }

    new_log(EXIT, NULL, 0);
    
    if (original)
        exit(0);

    else
        _exit(0);
}

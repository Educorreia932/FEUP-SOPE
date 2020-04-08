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

static log_info *l;
static int log_fd;

int main(int argc, char* argv[], char* envp[]) {
    char name[300]; 
    int folder_size = 0;
    int fd[2];
    char size_currentDir[50];

    // Checks if this one is the original process
    bool original = isOriginal(envp);

    //Signal Handler
    if(original)
        signal(SIGINT, handle_sigint); 

    //Check Flags
    flags* c = flags_constructor();

    if (argc > 9) {
        perror("Usage: simpledu -l [path] [-a] [-b] [-B size] [-L] [-S] [--max-depth=N]");
        exit(1);
    }

    parse_flags(argc, argv, c);

    //Group ID
    idgroup = getpgid(getpid());


    //Log file
    l = log_info_constructor();
    char log_file[50];
    get_log_filename(envp, log_file);

    
    if (original){
        log_fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0644);
        get_instance();
    }
    else {
        log_fd = open(log_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }

    if (log_fd == -1) {
        perror(log_file);
        exit(1);
    }
    
    //Checks if firstChild
    bool first_child = false;    


    // Open directory
    DIR *dirp;

    if ((dirp = opendir(c->path)) == NULL) {
        perror(c->path);
        exit(1);
    } 

    // Read contents of current directory
    struct dirent *direntp;

    //queue for fds
    Queue_t * fds_queue = new_queue();

    while ((direntp = readdir(dirp)) != NULL) {
        if (!strcmp(direntp->d_name, "..")) 
            continue;

        // Format path for each directory/file
        sprintf(name, "%s/%s", c->path, direntp->d_name);

        // Info
        struct stat stat_buf;

        // Stat if -L active
        if (c->dereference && stat(name, &stat_buf) == -1){ 
            perror("lstat ERROR");
            exit(1);
        }

        // Lstat if -L not active
        else if (lstat(name, &stat_buf) == -1) {
            perror("lstat ERROR");
            exit(1);
        }

        // Calculates size
        int size = calculateSize(stat_buf, c);


        // FILE (or symb link if -S)
        if (S_ISREG(stat_buf.st_mode) || (!c->dereference && S_ISLNK(stat_buf.st_mode) && c->all)) {
            // Format print with size
            folder_size += size;

            if (c->max_depth > 0 && c->all){
                char str[400];

                if (!c->bytes)
                    size = (size + c->size - 1) / c->size;

                sprintf(str, "%-7u %s\n", size, name);
                //new_log(ENTRY, log_fd, NULL, str);
                write(STDOUT_FILENO, str, strlen(str));
            }

            
        }
        
        // Directory
        else if (S_ISDIR(stat_buf.st_mode) ) {
            if (pipe(fd) < 0) 
                perror("Pipe error %s\n");  

            if (!strcmp(direntp->d_name, ".")){
                folder_size += size;
            }

            else {
                
                dup2(fd[WRITE], 999);
                
                queue_push_back(fds_queue, fd[READ]);

                pid_t pid = -1;
                pid = fork();
                

                if(original && !first_child)
                    first_child = true;   

                // Parent
                if (pid > 0) {
                    int child_size;

                    
                    
                    if(original)
                    {
                        char ready;
                        while(read(fd[READ], &ready, sizeof(char)) == 0);
                        idgroup = pid;
                    }
                    
                }

                // Child
                else if (pid == 0) {
                   //close(fd[READ]);
                    
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

                    char str[100];
                    //args_to_string(argv_, str);
                    //new_log(CREATE, log_fd, NULL, str);

        
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

    
    pid_t wpid;
    int status;
    int child_size;

    while ((wpid = wait(&status)) > 0);
    
    //Read from all children
    while(!queue_is_empty(fds_queue))
    {
       if(read(queue_pop(fds_queue), &child_size, sizeof(int)) < 0)
        {
            exit(1);
        }
        
        if(!c->separate_dirs)
            folder_size += child_size;
    }
    

    if (c->max_depth > 0) {
        int folder_aux;
        if (!c->bytes)
            folder_aux= (folder_size + c->size - 1) / c->size;

        sprintf(size_currentDir, "%-7u %s\n", folder_aux, c->path);

        write(STDOUT_FILENO, size_currentDir, strlen(size_currentDir));
    }


   write(999, &folder_size, sizeof(int));

    closedir(dirp); 
    close(fd[READ]);
    close(fd[WRITE]);

    exit(0);

}

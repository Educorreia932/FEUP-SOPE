#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

int main(int argc, char* argv[]) {
    int pipe_fd[2];
    char* filename = argv[1];

    pipe(pipe_fd);  

    // Read file
    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        perror("Couldn't open file.");
        exit(1);
    }

    pid_t pid = fork();

    // Parent
    if (pid > 0) {
        close(pipe_fd[READ]);

        int bytesRead;
        char buf[512];

        while ((bytesRead = read(fd, buf, 512)) > 0) {
            write(pipe_fd[WRITE], buf, bytesRead);
        }

        close(pipe_fd[WRITE]);
    }

    // Child
    else if (pid == 0) {
        close(pipe_fd[WRITE]);
        dup2(pipe_fd[READ], STDIN_FILENO);

        execlp("sort", "sort", NULL);
        perror("Couldn't execute sort.");
        exit(1);
    }

    else {
        perror("Couldn't create fork.");
        exit(1);
    }
    
    return 0;
}
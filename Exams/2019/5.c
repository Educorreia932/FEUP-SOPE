#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <string.h> 
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>

#define READ 0
#define WRITE 1

int main() {
    size_t BUFFER_SIZE = 256;
    
    int fd1[2];
    int fd2[2];

    pipe(fd1);
    pipe(fd2);

    pid_t pid = fork();

    if (pid == 0) {
        close(fd1[WRITE]);
        close(fd2[READ]);

        dup2(fd1[READ], STDIN_FILENO);    
        dup2(fd2[WRITE], STDOUT_FILENO);

        execlp("bc", "bc", "-qi", NULL);

        perror("Failed exec");

        exit(1);
    }

    close(fd1[READ]);
    close(fd2[WRITE]);       

    char aux[256];
    char* line = aux;

    int file = open("result.txt", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0644);

    FILE* f = fdopen(fd2[READ], "r");

    while (true) {
        getline(&line, &BUFFER_SIZE, stdin);

        if (line[0] == '\n')
            break;

        write(fd1[WRITE], line, strlen(line));

        getline(&line, &BUFFER_SIZE, f);

        line[strlen(line) - 1] = '\0'; // Replace the '\n'

        write(file, line, strlen(line));
        write(file, " = ", 3);
        
        getline(&line, &BUFFER_SIZE, f);

        write(file, line, strlen(line));
    }

    fclose(f);
    
    close(file);
    close(fd1[WRITE]);
    close(fd2[READ]);

    kill(pid, SIGTERM);
    wait(NULL);

    return 0;
}
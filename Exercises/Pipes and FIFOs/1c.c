#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define READ 0
#define WRITE 1 

int main() {
    int fd[2];
    pid_t pid;

    pipe(fd);

    pid = fork();

    // Father 
    if (pid > 0) {
        char* a[2];

        printf("x y? ");
        scanf("%s %s", a[0], a[1]);

        close(fd[READ]);
        write(fd[WRITE], &a, 2 * sizeof(char*));
        close(fd[WRITE]);
    }

    // Son
    else {
        char* b[2]; 

        close(fd[WRITE]);
        read(fd[READ], &b, 2 * sizeof(char*)); 

        int x = atoi(b[0]);
        int y = atoi(b[1]);

        printf("x + y = %d\n", x + y); 
        printf("x - y = %d\n", x - y); 
        printf("x * y = %d\n", x * y);  
        printf("x / y = %d\n", x / y); 

        close(fd[READ]); 
    }

   return 0;
}
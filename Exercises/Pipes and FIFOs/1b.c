#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1 

typedef struct  {
    int x;
    int y;
} numbers;

int main() {
    int fd[2];
    pid_t pid;

    pipe(fd);

    pid = fork();

    // Parent
    if (pid > 0) {
        numbers a;

        printf("x y? ");
        scanf("%d %d", &a.x, &a.y);

        close(fd[READ]);
        write(fd[WRITE], &a, sizeof(numbers));
        close(fd[WRITE]);
    }

    // Child
    else {
        numbers b;

        close(fd[WRITE]);
        read(fd[READ], &b, sizeof(numbers)); 

        printf("x + y = %d\n", b.x + b.y);    
        printf("x - y = %d\n", b.x - b.y); 
        printf("x * y = %d\n", b.x * b.y); 
        printf("x / y = %d\n", b.x / b.y); 

        close(fd[READ]); 
    }

   return 0;
}
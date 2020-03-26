#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1 

int main() {
    int fd1[2], fd2[2];
    pid_t pid;

    pipe(fd1);
    pipe(fd2);

    pid = fork();

    // Parent
    if (pid > 0) {
        int a[2];

        printf("x y? ");
        scanf("%d %d", &a[0], &a[1]);

        close(fd1[READ]);
        write(fd1[WRITE], a, 2 * sizeof(int));
        close(fd1[WRITE]);

        int results_a[4];

        close(fd2[WRITE]);
        read(fd2[READ], results_a, 4 * sizeof(int)); 
        close(fd2[READ]);

        printf("x + y = %d\n", results_a[0]); 
        printf("x - y = %d\n", results_a[1]); 
        printf("x * y = %d\n", results_a[2]); 
        printf("x / y = %d\n", results_a[3]); 
    }

    // Child
    else {
        int b[2]; 

        close(fd1[WRITE]);
        read(fd1[READ], b, 2 * sizeof(int)); 

        int results_b[4];

        results_b[0] = b[0] + b[1]; 
        results_b[1] = b[0] - b[1]; 
        results_b[2] = b[0] * b[1]; 
        results_b[3] = b[0] / b[1]; 

        close(fd1[READ]); 

        close(fd2[READ]);
        write(fd2[WRITE], results_b, 4 * sizeof(int));
        close(fd2[WRITE]);
    }

   return 0;
}
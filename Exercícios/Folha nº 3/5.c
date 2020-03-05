#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t pid;

    pid = fork(); 

    if (pid < 0) {
        printf("fork error");

        exit(1);
    } 
            
    else if (pid == 0) {
        pid = fork();

        if (pid < 0) {
            printf("fork error");

            exit(1);
        } 

        else if (pid == 0)
            printf("Hello ");

        else {
            int st;
            wait(&st);

            printf("my ");
        }
    }

    else {
        int st;
        wait(&st);

        printf("friends!\n");
    }

    exit(0);
}
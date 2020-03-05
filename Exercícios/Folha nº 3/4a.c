#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid;

    if ((pid = fork()) == 0) 
        printf("Hello");

    else
        printf("world !");

    return 0;
}
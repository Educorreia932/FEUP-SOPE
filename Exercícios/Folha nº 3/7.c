#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char prog[20];
    sprintf(prog, "%s.c", argv[1]); // Appends .c to the program name
    execlp("gcc", "gcc", prog, "-Wall", "-o", argv[1], NULL); // Compiles the program
    // printf(...);
    exit(0);
} 
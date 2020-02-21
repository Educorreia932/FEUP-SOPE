#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);

    char name [50]= ""; // Is it necessary to specify a size?

    for (int i = 2; i < argc; i++) {
        strcat(name, argv[i]);
        strcat(name, " ");
    }

    while (n) {
        printf("Hello %s!\n", name);
        n--;
    }

    return 0;
}   
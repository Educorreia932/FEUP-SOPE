#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char name [50]= ""; // Is it necessary to specify a size?

    for (int i = 1; i < argc; i++) {
        strcat(name, argv[i]);
        strcat(name, " ");
    }

    printf("Hello %s!\n", name);

    return 0;
}   
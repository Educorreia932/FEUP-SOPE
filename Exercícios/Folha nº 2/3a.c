#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[]) {
    FILE* file;
    char buffer[BUFFER_SIZE];

    if (argc != 2){
        printf("Usage: %s <filename>\n", argv[1]);

        return 1;
    }

    if ((file = fopen(argv[1],"r")) == NULL)
        return 2;

    while (fread(buffer, sizeof(char), 1, file) != 0)
        printf(buffer);

    printf("\n");

    return 0;
}
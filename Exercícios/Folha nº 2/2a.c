#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[]) {
    FILE* source, *destination;
    char buffer[BUFFER_SIZE];

    if (argc != 3){
        printf("Usage: %s <source> <destination>\n");

        return 1;
    }

    if ((source = fopen(argv[1],"r")) == NULL)
        return 2;

    if ((destination = fopen(argv[2],"w")) == NULL)
        return 3;

    while (fread(buffer, sizeof(char), 1, source) != 0)
        fwrite(buffer, sizeof(char), 1, destination);

    fclose(source);
    fclose(destination);

    return 0;
}
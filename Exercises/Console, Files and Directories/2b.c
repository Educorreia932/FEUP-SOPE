
#include <fcntl.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[]) {
    int source, destination, nr, nw;
    unsigned char buffer[BUFFER_SIZE];
    
    if (argc != 3) {
        printf("Usage: %s <source> <destination>\n", argv[0]);
        
        return 1;
    }

    if ((source = open(argv[1], O_RDONLY)) == -1) {
        perror(argv[1]);

        return 2;
    }

    if ((destination = open(argv[2], O_WRONLY | O_CREAT, 0644)) == -1) {
        perror(argv[2]);
        close(source);

        return 3;
    }

    while ((nr = read(source, buffer, BUFFER_SIZE)) != 0)
        if ((nw = write(destination, buffer, nr)) <= 0 || nw != nr) {
            perror(argv[2]);
            close(source);
            close(destination);

            return 4;
        }

    close(source);
    close(destination);

    return 0; 
}
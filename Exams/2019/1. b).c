
#include <stdio.h>
#include <unistd.h> 

int main() {
    for (int i=1; i<=3; i++) {
        fork();
    } 

    printf("Hello ");

    return 0;
}
    
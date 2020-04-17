#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define STDERR 2
#define NUMITER 10000

int N = 50000;

void* thrfunc(void * arg) {
    int *i = (int*) malloc(sizeof(int));
    *i = 0;

    fprintf(stderr, "Starting thread %s\n", (char *) arg);
    
    while (N > 1) {
        write(STDERR, arg, 1);
        N--;
        (*i)++;
    }

    return i;
}

int main() {
    pthread_t ta, tb;
    
    char ca = '1';
    char cb = '2';

    int* res1;
    int* res2;

    pthread_create(&ta, NULL, thrfunc, &ca);
    pthread_create(&tb, NULL, thrfunc, &cb);
    
    pthread_join(ta, (void*)&res1);
    pthread_join(tb, (void*)&res2);

    printf("\nResult 1: %d\n", *res1);
    printf("Result 2: %d\n", *res2);

    free(res1);
    free(res2);

    return 0;
} 
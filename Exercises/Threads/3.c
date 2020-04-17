#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

void *PrintHello(void* threadnum) {
    printf("Hello from thread no. %d!\n", *(int *) threadnum);
    free(threadnum);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int t;
    
    for (t = 0; t < NUM_THREADS; t++){
        printf("Creating thread %d\n", t);

        int* arg = malloc(sizeof(int));
        *arg = t;

        pthread_create(&threads[t], NULL, PrintHello, arg);
    }

    pthread_exit(0);
}
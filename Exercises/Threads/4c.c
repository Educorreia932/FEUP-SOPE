#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_THREADS 10

void* thrfunc(void* arg) {
    sleep(1);

    pthread_t tid = pthread_self();
    printf("%lu\n", (unsigned long) tid);

    pthread_exit(arg); // Thread number
}

int main() {
    pthread_t threads[NUM_THREADS];
    int threadNums[NUM_THREADS];
    int* res;

    for (int i = 0; i < NUM_THREADS; i++) {
        threadNums[i] = i+ 1;
        pthread_create(&threads[i], NULL, thrfunc, (void*) &threadNums[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], (void*) &res);
        printf("Thread %d returned %d.\n", threadNums[i], *res);    
    }
    
    pthread_exit(0);
}
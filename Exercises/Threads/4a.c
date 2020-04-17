#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 10

void* thrfunc(void* arg) {
    sleep(1);
    pthread_t tid = pthread_self();

    printf("%lu\n", (unsigned long) tid);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) 
        pthread_create(&threads[i], NULL, thrfunc, (void*) &(i));

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}
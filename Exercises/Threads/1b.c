#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define STDERR 2
#define NUMITER 10000

void* thrfunc(void * arg) {
    int i;
    fprintf(stderr, "Starting thread %s\n", (char *) arg);
    
    for (i = 1; i <= NUMITER; i++) 
        write(STDERR, arg, 1);
    
    return NULL;
}

int main() {
    pthread_t ta, tb;
    
    char ca = '1';
    char cb = '2';

    int ia = 1;
    int ib = 2;

    pthread_create(&ta, NULL, thrfunc, &ca);
    pthread_create(&tb, NULL, thrfunc, &cb);
    
    pthread_join(ta, NULL);
    pthread_join(tb, NULL);

    return 0;
} 
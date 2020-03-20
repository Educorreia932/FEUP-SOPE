#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int increment;

void usr_handler(int signo) {
    if (signo == SIGUSR1)
        increment = 1;

    else if (signo == SIGUSR2)
        increment = -1;
}

int main(void) {
    int v = 0;

    struct sigaction action;
    action.sa_handler = usr_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGUSR1, &action, NULL) < 0) {
        fprintf(stderr, "Unable to install SIGUSR1 handler\n");
        exit(1);
    }

    if (sigaction(SIGUSR2, &action, NULL) < 0) {
        fprintf(stderr, "Unable to install SIGUSR2 handler\n");
        exit(1);
    }

    while (1) {
        printf("v = %d\n", v);
        sleep(1);

        v += increment;
    }

    exit(0);
}
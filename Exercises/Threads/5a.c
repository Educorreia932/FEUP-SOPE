#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float a, b;
    char operation;
} operands;

void* thrfunc(void* arg) {
    operands op = *(operands*) arg;
    float* result = malloc(sizeof(float));

    switch (op.operation) {
        case '+':
            *result = op.a + op.b;
            break;
        
        case '-':
            *result = op.a - op.b;
            break;

        case '*':
            *result = op.a * op.b;
            break;
        
        case '/':
            *result = op.a / op.b;
            break;

        default:
            break;
    }

    pthread_exit(result);
}

int main() {
    pthread_t threads[4];
    operands op;

    scanf("%f%f", &op.a, &op.b);

    char operations[] = {'+', '-', '*', '/'};

    for (int i = 0; i < 4; i++) {
        op.operation = operations[i];
        pthread_create(&threads[0], NULL, thrfunc, &op);
    }

    for (int i = 0; i < 4; i++) {
        void* result;
        pthread_join(threads[i], &result);

        printf("%f %c %f = %f\n", op.a, operations[i], op.b, *(float*) result);

        free(result);
    }

    return 0;
}
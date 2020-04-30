#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#define MAX_THREADS 10
#define MAX_STR 100
#define BUF_SIZE 512

void parse_request(char *buffer, char *array[], size_t size);
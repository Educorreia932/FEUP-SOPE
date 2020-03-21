#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h> 
#include <stdbool.h>
#include <limits.h>

#define DEFAULT_BLOCK_SIZE 1024

typedef struct {
    char* path; 
    bool all; // Also include files
    bool bytes; // Real number of bytes
    unsigned int size; // Block size in bytes
    bool dereference; // Follow symbolic links 
    bool separate_dirs; // Don't include 
    unsigned int max_depth; // Max depth to show
} flags;

void parse_flags(int argc, char* argv[], flags* c);

void print_flags(flags* c);
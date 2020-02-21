#include <stdio.h>
#include <string.h>

int main(int argc, char **argv, char **envp) {
    for (char** env = envp; *env != 0; env++) {
        char* thisEnv = *env;
        char variable[5];

        strncpy(variable, thisEnv, 5);

        if (!strcmp(variable, "USER=")) {
            thisEnv += 5;
            printf("Hello %s!\n", thisEnv);    
        }
    }

    return 0;
}
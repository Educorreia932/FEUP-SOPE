#include "utils.h"

#include <stdio.h>

void parse_request(char *buffer, char *array[], size_t size){

    char * pch;
    pch = strtok(buffer, " ,");
    
    int cnt = 0;
    array[0] = pch;
    while(pch != NULL) {
      
        cnt++;
        pch = strtok(NULL, " ,");
        if (cnt == 1){
            array[1] = pch;
        }
        else if (cnt == 2) {
            array[2] = pch;
        }
    }
}
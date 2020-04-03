#include "utils.h"

bool check_folder(char* name) {
    const int path_length = strlen(name);

    bool current_folder = name[path_length - 1] == '.' && name[path_length - 2] == '/'; 
    bool parent_folder =  name[path_length - 1] == '.' && name[path_length - 2] == '.' && name[path_length - 3] == '/';

    return /*current_folder ||*/ parent_folder;
}
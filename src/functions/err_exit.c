#include "../../headers/err_exit.h"
#include <stdio.h>
#include <stdlib.h>

void err_exit(char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

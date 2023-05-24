#include "err_exit.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void err_exit(char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}
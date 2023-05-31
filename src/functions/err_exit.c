#include "../../headers/err_exit.h"

void err_exit(char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

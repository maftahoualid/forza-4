#ifndef _INITGAME_HH
#define _INITGAME_HH

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "err_exit.h"
#include "smfr.h"

void check_args(int argc, char **argv);
void forza4(int sem_id, int* shm_ptr, int rows, int cols);
int check_winner(int* shm_ptr, int rows, int cols);
void usage();
void close_server();

#endif
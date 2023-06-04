#ifndef _INITGAME_HH
#define _INITGAME_HH

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "err_exit.h"


void check_server_args(int argc, char **argv);
void check_client_args(int argc, char **argv);
int check_winner(int* shm_ptr, int rows, int cols);
void usage();
void close_server();

#endif
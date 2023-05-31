#ifndef _PLAY_HH
#define _PLAY_HH

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sem.h>
#include "init_game.h"
#include "smfr.h"

void forza4(int sem_id, int* shm_ptr, int rows, int cols);
int check_winner(int* shm_ptr, int rows, int cols);

#endif
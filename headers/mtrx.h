#ifndef _MTRX_HH
#define _MTRX_HH

#include <stdio.h>



void init_matrix(void *shm_ptr, int rows, int cols);
void print_matrix(void *shm_ptr, int rows, int cols);

#endif
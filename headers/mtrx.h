#ifndef _MTRX_HH
#define _MTRX_HH

#include <stdio.h>

struct matrix {
    int rows;
    int cols;
    char sym1;
    char sym2;
};

void init_matrix(void *shm_ptr, int rows, int cols);
void print_matrix(void *shm_ptr, int rows, int cols);

#endif
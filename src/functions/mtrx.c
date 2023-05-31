#include "../../headers/mtrx.h"

void init_matrix(void *shm_ptr, int rows, int cols){
    int (*matrix)[cols] = (void*)shm_ptr;
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            matrix[i][j] = 0;
        }
    }
}

void print_matrix(void *shm_ptr, int rows, int cols){
    int (*matrix)[cols] = (void*)shm_ptr;
    for(int i=1; i<rows; i++){
        for(int j=1; j<cols; j++){
            printf("[%d]", matrix[i][j]);
        }
        printf("\n");
    }
}
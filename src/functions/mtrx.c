#include "../../headers/mtrx.h"

void init_matrix(void *shm_ptr, int rows, int cols){
    int (*matrix)[cols] = (void*)shm_ptr;
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            matrix[i][j] = 0;
        }
    }
}
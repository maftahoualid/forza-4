// cd /home/xwally/Scrivania/forza-4
// gcc -Wall -Wextra ./src/F4Client.c ./src/functions/err_exit.c ./src/functions/init_game.c -o ./F4Client.out && ./F4Client.out 

#include "../headers/base.h"
#include "../headers/err_exit.h"
#include "../headers/init_game.h"

int main(){

    int shm_key;
    int shm_id;
    int *shm_ptr;

    int sem_key;
    int sem_id;
    int sem_num = 10; 

    int msq_key;
    int msq_id;

    int rows, cols;

    sem_key = ftok("../.",'b');
    printf("[CLIENT DEBUG] shm_key: %d\n",sem_key);
    sem_id = get_sem(sem_key,sem_num);
    printf("[CLIENT DEBUG] shm_id: %d\n",sem_id);

    signal_sem(sem_id, 0);
    printf("[CLIENT DEBUG]: Client creato\n");

    wait_sem(sem_id, 1);
    struct matrix_dim dim1;
    msq_key = ftok("../.", 'c');
    msq_id = msgget(msq_key, IPC_CREAT | 0666 );
    msgrcv(msq_id,&dim1,sizeof(int)*2,(long)1.0,0);
    printf("[%d,%d]",dim1.rows,dim1.cols);

    shm_key = ftok("../.",'a');
    shm_id = get_shm(shm_key, sizeof(int[dim1.rows][dim1.cols]));
    shm_ptr = at_shm(shm_id, 0);

    /*while (1){
        wait_sem(sem_id);
        print_matrix(shm_ptr,dim.rows,dim.cols);
    }*/
    
    return 0;
}

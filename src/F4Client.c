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

    

    shm_key = ftok("../.",'a');
    printf("[SERVER DEBUG] shm_key: %d\n",shm_key);
    shm_id = get_shm(shm_key, sizeof(int[rows][cols]));
    printf("[SERVER DEBUG] shm_id: %d\n",shm_id);
    shm_ptr = at_shm(shm_id, 0);
    printf("[SERVER DEBUG] shm_ptr: %p\n", shm_ptr);

    sem_key = ftok("../.",'b');
    printf("[CLIENT DEBUG] shm_key: %d\n",sem_key);
    sem_id = get_sem(sem_key,sem_num);
    printf("[CLIENT DEBUG] shm_id: %d\n",sem_id);

    signal_sem(sem_id);
    printf("[CLIENT DEBUG]: Client creato\n");

    while (1){
        wait_sem(sem_id);
        print_matrix()
    }
    
    
    return 0;
}
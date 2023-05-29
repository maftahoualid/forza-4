// cd /home/xwally/Scrivania/forza-4
// gcc -Wall -Wextra ./src/F4Server.c ./src/functions/err_exit.c ./src/functions/init_game.c -o ./F4Server.out && ./F4Server.out 6 7 x o

#include "../headers/base.h"
#include "../headers/init_game.h"
#include "../headers/err_exit.h"


int main(int argc, char **argv) {

    /* CONTROLLO VALIDITA' DEI PARAMETRI */
    check_args(argc, argv);
    
    /* DO UN NOME AI PARAMETRI */
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    signal(SIGINT, handle_sigint);
    signal(SIGALRM, handle_sigalrm);

    int shm_key; 
    int *shm_ptr; // PUNTATORE A MEMORIA CONDIVISA
    int shm_id; // ID MEMORIA CONDIVISA

    int sem_key;
    int sem_id; // ID SEMAFORI
    int sem_num = 10; // NUMERO SEMAFORI
    
    shm_key = ftok("../.",'a');
    printf("[SERVER DEBUG] shm_key: %d\n",shm_key);
    // CREO LA MEMORIA CONDIVISA //
    shm_id = get_shm(shm_key, sizeof(int[rows][cols]));
    printf("[SERVER DEBUG] shm_id: %d\n",shm_id);
    // FACCIO L'ATTACH DELLA MEMORIA //
    shm_ptr = at_shm(shm_id, 0);
    printf("[SERVER DEBUG] shm_ptr: %p\n", shm_ptr);
    // INIZIALIZZO LA MATRICE //
    init_matrix(shm_ptr,rows,cols);

    // STAMPO LA MATRICE // 
    print_matrix(shm_ptr,rows,cols);

    sem_key = ftok("../.",'b');
    printf("[SERVER DEBUG] sem_key: %d\n", sem_key);
    // CREO IL SET DI SEMAFORI //
    sem_id = get_sem(sem_key,sem_num);
    printf("[SERVER DEBUG] sem_id: %d\n", sem_id);
    // INIZIALIZZO IL SET DI SEMAFORI //
    ctl_sem(sem_id,sem_num);

    printf("[SERVER DEBUG] In attesa che i due client si colleghino.\n");
    wait_sem(sem_id);
    printf("[SERVER DEBUG] Client 1 collegato\n");
    wait_sem(sem_id);
    printf("[SERVER DEBUG] Client 2 collegato\n");

    forza4(sem_id,shm_ptr,rows,cols);

    // FACCIO IL DETACH DELLA MEMORIA // 
    dt_shm(shm_ptr);

    // RIMUOVO LA MEMORIA CONDIVISA // 
    rm_shm(shm_id);

    return 0;
}

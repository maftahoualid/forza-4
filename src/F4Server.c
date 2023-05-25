#include "../headers/base.h"
#include "../headers/init_game.h"
#include "../headers/err_exit.h"

// F4Server 6 7 O X
int main(int argc, char **argv) {

    /* CONTROLLO VALIDITA' DEI PARAMETRI */
    check_args(argc, argv);
    
    /* DO UN NOME AI PARAMETRI */
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    int shm_key; 
    int *shm_ptr; // PUNTATORE A MEMORIA CONDIVISA
    int shm_id; // ID MEMORIA CONDIVISA

    int sem_key;
    int sem_id; // ID SEMAFORI
    int sem_num = 10; // NUMERO SEMAFORI
    
    shm_key = ftok("../.",'a');
    // CREO LA MEMORIA CONDIVISA //
    shm_id = get_shm(shm_key, sizeof(int[rows][cols]));

    // FACCIO L'ATTACH DELLA MEMORIA //
    shm_ptr = at_shm(shm_id, 0);

    // INIZIALIZZO LA MATRICE //
    init_matrix(shm_ptr,rows,cols);

    // STAMPO LA MATRICE // 
    print_matrix(shm_ptr,rows,cols);

    sem_key = ftok("../.",'b');
    // CREO IL SET DI SEMAFORI //
    sem_id = get_sem(sem_key,sem_num);

    // INIZIALIZZO IL SET DI SEMAFORI //
    ctl_sem(sem_id,sem_num);

    // FACCIO IL DETACH DELLA MEMORIA // 
    dt_shm(shm_ptr);

    // RIMUOVO LA MEMORIA CONDIVISA // 
    rm_shm(shm_id);

    return 0;
}
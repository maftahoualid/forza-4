
#include <signal.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>


#include "../headers/err_exit.h"
#include "../headers/hndl_signals.h"
#include "../headers/init_game.h"
#include "../headers/ms_queue.h"
#include "../headers/mtrx.h"
#include "../headers/sh_mem.h"
#include "../headers/smfr.h"

int main(int argc, char **argv) {


    // CONTROLLO VALIDITA' DEI PARAMETRI //
    printf("[SERVER DEBUG] CONTROLLO VALIDITA' DEI PARAMETRI\n");
    check_server_args(argc, argv);
    // DO UN NOME AI PARAMETRI //
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    char *sym1 = argv[3]; 
    char *sym2 = argv[4]; 


    // GESTISCO I SEGNALI //
    printf("[SERVER DEBUG] GESTISCO I SEGNALI\n");
    signal(SIGINT, handle_sigint); //GESTISCO LA PRESSIONE DEL CTRL-C
    signal(SIGALRM, handle_sigalrm); //GESTISCO IL TIMER PER IL SECONDO CTRL-C


    int shm_key; // CHIAVE MEMORIA CONDIVISA //
    int *shm_ptr; // PUNTATORE A MEMORIA CONDIVISA //
    int shm_id; // ID MEMORIA CONDIVISA //


    int sem_key; // CHIAVE SET SEMAFORI //
    int sem_id; // ID SET SEMAFORI //
    int sem_num = 10; // NUMERO SEMAFORI //


    int msq_key; // CHIAVE CODA MESSAGGI //
    int msq_id; // ID CODA MESSAGGI //


    /* =========== memoria condivisa =========== */
    // CREO UNA CHIAVE PER LA MEMORIA CONDIVISA //
    printf("[SERVER DEBUG] CREO UNA CHIAVE PER LA MEMORIA CONDIVISA\n");
    shm_key = ftok("../.",'a');
    // CREO LA MEMORIA CONDIVISA //
    printf("[SERVER DEBUG] CREO LA MEMORIA CONDIVISA\n");
    shm_id = get_shm(shm_key, sizeof(int[rows][cols]));
    
    // FACCIO L'ATTACH DELLA MEMORIA //
    printf("[SERVER DEBUG] FACCIO L'ATTACH DELLA MEMORIA\n");
    shm_ptr = at_shm(shm_id);


    /* ======= matrice ======= */
    // INIZIALIZZO LA MATRICE //
    printf("[SERVER DEBUG] INIZIALIZZO LA MATRICE\n");
    init_matrix(shm_ptr,rows,cols);


    /* =========== set di semafori =========== */
    // CREO UNA CHIAVE PER IL SET DI SEMAFORI //
    printf("[SERVER DEBUG] CREO UNA CHIAVE PER IL SET DI SEMAFORI\n");
    sem_key = ftok("../.",'b');
    // CREO IL SET DI SEMAFORI //
    printf("[SERVER DEBUG] CREO IL SET DI SEMAFORI\n");
    sem_id = get_sem(sem_key,sem_num);
    // INIZIALIZZO IL SET DI SEMAFORI //
    printf("[SERVER DEBUG] INIZIALIZZO IL SET DI SEMAFORI\n");
    init_sem(sem_id);
    

    // MI BLOCCO FINCHÈ SEM 0 NON VA A ZERO // 
    printf("[SERVER DEBUG] MI BLOCCO FINCHÈ SEM 0 NON VA A ZERO\n");
    zero_sem(sem_id,0); 


    // CREO LA CODA DI MESSAGGI //
    printf("[SERVER DEBUG] CREO LA CODA DI MESSAGGI\n");
    msq_key = ftok("../.", 'c');
    msq_id = get_msq(msq_key);

    // CREO IL MESSAGGIO AL CLIENT 1 //
    printf("[SERVER DEBUG] CREO IL MESSAGGIO AL CLIENT 1\n"); 
    struct matrix_dim dim1 = {(long)1.0, rows, cols, *sym1};
    // INVIO IL MESSAGGIO AL CLIENT 1 //
    printf("[SERVER DEBUG] INVIO IL MESSAGGIO AL CLIENT 1\n");
    send_msg(msq_id,&dim1);

    // CREO IL MESSAGGIO AL CLIENT 2 //
    printf("[SERVER DEBUG] CREO IL MESSAGGIO AL CLIENT 2\n");
    struct matrix_dim dim2 = {(long)1.0, rows, cols, *sym2};
    // INVIO IL MESSAGGIO AL CLIENT 2 //
    printf("[SERVER DEBUG] INVIO IL MESSAGGIO AL CLIENT 2\n");
    send_msg(msq_id,&dim2);


    //  //



    // GESTIONE DEL GIOCO E DELLA VITTORIA //
    printf("[SERVER DEBUG]  GESTIONE DEL GIOCO E DELLA VITTORIA\n");
    forza4(sem_id,shm_ptr,rows,cols);


    // FACCIO IL DETACH DELLA MEMORIA // 
    printf("[SERVER DEBUG] FACCIO IL DETACH DELLA MEMORIA\n");
    dt_shm(shm_ptr);


    // RIMUOVO LA MEMORIA CONDIVISA // 
    printf("[SERVER DEBUG] RIMUOVO LA MEMORIA CONDIVISA\n");
    rm_shm(shm_id);

    return 0;
}

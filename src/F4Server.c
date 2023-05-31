
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
    /* CONTROLLO VALIDITA' DEI PARAMETRI */
    check_args(argc, argv);
    /* DO UN NOME AI PARAMETRI */
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    // GESTISCO I SEGNALI
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
    shm_key = ftok("../.",'a');
    printf("[SERVER DEBUG] shm_key: %d\n",shm_key);
    // CREO LA MEMORIA CONDIVISA //
    shm_id = get_shm(shm_key, sizeof(int[rows][cols]));
    printf("[SERVER DEBUG] shm_id: %d\n",shm_id);
    // FACCIO L'ATTACH DELLA MEMORIA //
    shm_ptr = at_shm(shm_id);
    printf("[SERVER DEBUG] shm_ptr: %p\n", shm_ptr);

    /* ======= matrice ======= */
    // INIZIALIZZO LA MATRICE //
    init_matrix(shm_ptr,rows,cols);
    // STAMPO LA MATRICE // 
    print_matrix(shm_ptr,rows,cols);

    /* =========== set di semafori =========== */
    // CREO UNA CHIAVE PER IL SET DI SEMAFORI //
    sem_key = ftok("../.",'b');
    printf("[SERVER DEBUG] sem_key: %d\n", sem_key);
    // CREO IL SET DI SEMAFORI //
    sem_id = get_sem(sem_key,sem_num);
    printf("[SERVER DEBUG] sem_id: %d\n", sem_id);
    // INIZIALIZZO IL SET DI SEMAFORI //
    ctl_sem(sem_id,sem_num);
    
    
    // ASPETTO IL PRIMO CLIENT // 
    printf("[SERVER DEBUG] In attesa che i due client si colleghino.\n");
    // MI BLOCCO PER ASPETTARE IL PRIMO CLIENT [WAIT 0] //
    wait_sem(sem_id, 0);
    // IL PRIMO CLIENT MI HA SBLOCCATO [SIGNAL 0] //
    printf("[SERVER DEBUG] Client 1 collegato\n");
    

    // CREO IL MESSAGGIO // 
    struct matrix_dim dim1 = {(long)1.0, rows, cols};
    // CREO LA CODA DI MESSAGGI //
    msq_key = ftok("../.", 'c');
    msq_id = msgget(msq_key, IPC_CREAT | 0666 );
    // INVIO IL MESSAGGIO //
    msgsnd(msq_id,&dim1,sizeof(int)*2,0);

    
    // ASPETTO IL SECONDO CLIENT //
    wait_sem(sem_id, 0);
    // IL SECONDO CLIENT MI HA SBLOCCATO //
    printf("[SERVER DEBUG] Client 2 collegato\n");

    
    // CREO IL MESSAGGIO
    struct matrix_dim dim2 = {(long)1.0, rows, cols};
    // INVIO IL MESSAGGIO //
    msgsnd(msq_id,&dim2,sizeof(int)*2,0);


    // SBLOCCO IL SECONDO CLIENT //
    signal_sem(sem_id, 1);


    // GESTIONE DEL GIOCO E DELLA VITTORIA //
    forza4(sem_id,shm_ptr,rows,cols);


    // FACCIO IL DETACH DELLA MEMORIA // 
    dt_shm(shm_ptr);


    // RIMUOVO LA MEMORIA CONDIVISA // 
    rm_shm(shm_id);

    return 0;
}

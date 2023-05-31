
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

int main(){
    int shm_key; // CHIAVE MEMORIA CONDIVISA //
    int *shm_ptr; // PUNTATORE A MEMORIA CONDIVISA //
    int shm_id; // ID MEMORIA CONDIVISA //
    int sem_key; // CHIAVE SET SEMAFORI //
    int sem_id; // ID SET SEMAFORI //
    int sem_num = 10; // NUMERO SEMAFORI //
    int msq_key; // CHIAVE CODA MESSAGGI //
    int msq_id; // ID CODA MESSAGGI //


    // CREO UNA CHIAVE PER IL SET DI SEMAFORI //
    sem_key = ftok("../.",'b');
    printf("[CLIENT DEBUG] shm_key: %d\n",sem_key);
    // CREO IL SET DI SEMAFORI //
    sem_id = get_sem(sem_key,sem_num);
    printf("[CLIENT DEBUG] shm_id: %d\n",sem_id);


    // SBLOCCO IL SERVER [SIGNAL 0] //
    signal_sem(sem_id, 0);
    // CLIENT CREATO //
    printf("[CLIENT DEBUG]: Client creato\n");

    // CREO LA STRUTTURA PER SALVARE IL MESSAGGIO //
    struct matrix_dim dim1;
    // MI COLLEGO ALLA CODA DI MESSAGGI //
    msq_key = ftok("../.", 'c');
    msq_id = msgget(msq_key, IPC_CREAT | 0666 );
    // RICEVO IL MESSAGGIO INVIATO DAL SERVER //
    // [!!!] una volta letto un messaggio, questo viene tolto dalla coda
    msgrcv(msq_id,&dim1,sizeof(int)*2,(long)1.0,0);
    // STAMPO I CAMPI RIGA E COLONNA DEL MESSAGGIO RICEVUTO
    printf("[CLIENT DEBUG] [%d,%d]",dim1.rows,dim1.cols);


    // MI COLLEGO ALLA MEMORIA CONDIVISA 
    shm_key = ftok("../.",'a');
    shm_id = get_shm(shm_key, sizeof(int[dim1.rows][dim1.cols]));
    // LA MAPPO SUL PROCESSO CLIENT //
    shm_ptr = at_shm(shm_id);


    // FACCIO IL DETACH DELLA MEMORIA CONDIVISA // 
    dt_shm(shm_ptr);
    
    return 0;
}

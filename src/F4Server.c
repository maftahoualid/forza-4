
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

    {// CONTROLLO VALIDITA' DEI PARAMETRI //
    printf("[SERVER DEBUG] CONTROLLO VALIDITA' DEI PARAMETRI\n");
    check_server_args(argc, argv);
    }
    
    
    // DO UN NOME AI PARAMETRI //
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    char *sym1 = argv[3]; 
    char *sym2 = argv[4]; 
    int shm_key; // CHIAVE MEMORIA CONDIVISA //
    int *shm_ptr; // PUNTATORE A MEMORIA CONDIVISA //
    int shm_id; // ID MEMORIA CONDIVISA //
    int sem_key; // CHIAVE SET SEMAFORI //
    int sem_id; // ID SET SEMAFORI //
    int sem_num = 10; // NUMERO SEMAFORI //
    int msq_key; // CHIAVE CODA MESSAGGI //
    int msq_id; // ID CODA MESSAGGI //
    int win=0;
    int size=0;
    // SEMAFORI: 1,1,1,1,0 //


    {/* =========== segnali =========== */
    printf("[SERVER DEBUG] GESTISCO I SEGNALI\n");
    signal(SIGINT, handle_sigint); //GESTISCO LA PRESSIONE DEL CTRL-C
    }

    {/* =========== set di semafori =========== */
    // CREO UNA CHIAVE PER IL SET DI SEMAFORI //
    printf("[SERVER DEBUG] CREO UNA CHIAVE PER IL SET DI SEMAFORI\n");
    sem_key = ftok("../.",'b');
    // CREO IL SET DI SEMAFORI //
    printf("[SERVER DEBUG] CREO IL SET DI SEMAFORI\n");
    sem_id = create_sem(sem_key,sem_num);
    // INIZIALIZZO IL SET DI SEMAFORI //
    printf("[SERVER DEBUG] INIZIALIZZO IL SET DI SEMAFORI\n");
    init_sem(sem_id);
    }

    {/* =========== coda di messaggi =========== */
    // CREO LA CODA DI MESSAGGI //
    printf("[SERVER DEBUG] CREO LA CODA DI MESSAGGI\n");
    msq_key = ftok("../.", 'c');
    msq_id = create_msq(msq_key);
    }

    struct matrix_dim dim1 = {(long)1, rows, cols, *sym1, *sym2};
    
    struct matrix_dim dim2 = {(long)1, rows, cols, *sym2, *sym1};

    {/* =========== memoria condivisa =========== */
    // CREO UNA CHIAVE PER LA MEMORIA CONDIVISA //
    printf("[SERVER DEBUG] CREO UNA CHIAVE PER LA MEMORIA CONDIVISA\n");
    shm_key = ftok("../.",'a');
    // CREO LA MEMORIA CONDIVISA //
    printf("[SERVER DEBUG] CREO LA MEMORIA CONDIVISA\n");
    shm_id = create_shm(shm_key, sizeof(int[rows][cols]));
    // FACCIO L'ATTACH DELLA MEMORIA //
    printf("[SERVER DEBUG] FACCIO L'ATTACH DELLA MEMORIA\n");
    shm_ptr = at_shm(shm_id);
    }

    {/* =========== matrice =========== */
    // INIZIALIZZO LA MATRICE //
    printf("[SERVER DEBUG] INIZIALIZZO LA MATRICE\n");
    init_matrix(shm_ptr,rows,cols);
    }

    {/* aspetto connessione dei client */
    // MI BLOCCO FINCHÈ SEM 0 NON VA A ZERO // 
    // ASPETTO CHE I DUE CLIENT SI COLLEGHINO AI SEMAFORI //
    printf("[SERVER DEBUG] MI BLOCCO FINCHÈ SEM 0 NON VA A ZERO\n");
    // 1,1,1,1,0 //
/**/zero_sem(sem_id,0);
    // PER VIA DI CLIENT1: 0,1,1,1,0
    printf("Si è collegato il primo giocatore\n");
/**/inc_sem(sem_id,0);
    // 1,1,1,1
/**/zero_sem(sem_id,0);
    // PER VIA DI CLIENT2: 0,1,1,1,0
    printf("Si è collegato il secondo giocatore\n");
    }

    {/* invio messaggi ai client */   
    // INVIO IL MESSAGGIO AL CLIENT 1 //
    printf("[SERVER DEBUG] INVIO IL MESSAGGIO AL CLIENT 1\n");
/**/size = sizeof(struct matrix_dim)-sizeof(long);
    if(msgsnd(msq_id,&dim1,size,0)==-1){
        err_exit("[ERROR] msgsnd");
    }
    // INVIO IL MESSAGGIO AL CLIENT 2 //
    printf("[SERVER DEBUG] INVIO IL MESSAGGIO AL CLIENT 2\n");
/**/size = sizeof(struct matrix_dim)-sizeof(long);
    if(msgsnd(msq_id,&dim2,size,0)==-1){
        err_exit("[ERROR] msgsnd");
    }
    }

    struct player player;

    do {
        /* attesa mossa */
        printf("[CLIENT DEBUG] aspetto la mossa del giocatore\n");
        zero_sem(sem_id,0);
        size = sizeof(struct player)-sizeof(long);
        if(msgrcv(msq_id,&player,size,(long)2,0)==-1){
            err_exit("[ERROR] msgsnd");
        }


        win = check_winner(shm_ptr,rows,cols);
        printf("win: %d\n",win);
        if(win==1){
            printf("Player %s, ha vinto!\n", player.name);
            semctl(sem_id,3,SETVAL,1);
        } else {
            printf("Player %s, non hai vinto\n",player.name);
            /* sblocco client 2 */
            inc_sem(sem_id,1);
        }
    } while(win==0);


    {// FACCIO IL DETACH DELLA MEMORIA // 
    printf("[SERVER DEBUG] FACCIO IL DETACH DELLA MEMORIA\n");
    dt_shm(shm_ptr);
    }

    {// RIMUOVO LA MEMORIA CONDIVISA // 
    printf("[SERVER DEBUG] RIMUOVO LA MEMORIA CONDIVISA\n");
    rm_shm(shm_id);
    }

    return 0;
}

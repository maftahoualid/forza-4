
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

int main(int argc, char **argv){
    int shm_key; // CHIAVE MEMORIA CONDIVISA //
    int *shm_ptr; // PUNTATORE A MEMORIA CONDIVISA //
    int shm_id; // ID MEMORIA CONDIVISA //
    int sem_key; // CHIAVE SET SEMAFORI //
    int sem_id; // ID SET SEMAFORI //
    int sem_num = 10; // NUMERO SEMAFORI //
    int msq_key; // CHIAVE CODA MESSAGGI //
    int msq_id; // ID CODA MESSAGGI //
    int row=0, col=0; // LA COLONNA DOVE INSERIRE IL DISCHETTO //
    struct matrix_dim dim; // STRUTTURA PER SALVARE IL MESSAGGIO //
    struct player player;
    int turno = 1;
    // SEMAFORI: 1,1,1,1,0 //

    { /* ARGOMENTI */
    check_client_args(argc,argv);
    }

    { /* SEMAFORI */
    // CREO UNA CHIAVE PER IL SET DI SEMAFORI //
    sem_key = ftok("../.",'b');
    printf("[CLIENT DEBUG] shm_key: %d\n",sem_key);
    // CREO IL SET DI SEMAFORI //
    sem_id = get_sem(sem_key,sem_num);
    printf("[CLIENT DEBUG] shm_id: %d\n",sem_id);

    printf("[CLIENT DEBUG]: Client creato\n");
    // CLIENT1: 1,1,1,1,0
    // CLIENT2: 0,1,1,1,0
/**/dec_sem(sem_id,0);
    // 1,1,1,1
    }

    { /* MESSAGGIO */
    // MI COLLEGO ALLA CODA DI MESSAGGI //
    msq_key = ftok("../.", 'c');
    msq_id = get_msq(msq_key);
    // RICEVO IL MESSAGGIO INVIATO DAL SERVER //
    // [!!!] una volta letto un messaggio, questo viene tolto dalla coda
    printf("[CLIENT DEBUG] aspetto il messaggio\n");
/**/int size = sizeof(struct matrix_dim)-sizeof(long);
    if(msgrcv(msq_id,&dim,size,(long)1,0)==-1){
        err_exit("[ERROR] msgsnd");
    }
    // STAMPO I CAMPI RIGA E COLONNA DEL MESSAGGIO RICEVUTO
    printf("[CLIENT DEBUG] messaggio ricevuto : [%d,%d] [%c]\n",dim.rows,dim.cols,dim.sym1);
    }

    { /* MEMORIA CONDIVISA */
    // MI COLLEGO ALLA MEMORIA CONDIVISA 
    shm_key = ftok("../.",'a');
    shm_id = get_shm(shm_key, sizeof(int[dim.rows][dim.cols]));
    // LA MAPPO SUL PROCESSO CLIENT //
    shm_ptr = at_shm(shm_id);
    }

    int (*matrix)[dim.cols] = (void*)shm_ptr;

    do{

        { /* creo il messaggio */
        player.mtype=(long)2;
        printf("TIPO: %ld\n", player.mtype);
        strcpy(player.name, argv[1]);
        printf("NOME: %s\n", player.name);
        player.sym=dim.sym1;
        printf("SIMBOLO MIO: %c\n", player.sym);
        printf("SIMBOLO ALTRO: %c\n", dim.sym2);
        }

        {// CHIEDO L'INSERIMENTO DELLA MOSSA //
        do { 
            printf("Inserisci una colonna (tra 1 e %d): ",dim.cols);
            // SALVO LA MOSSA //
            scanf("%d", &col);
            if(matrix[0][col-1]==1){
                printf("La colonna è piena!\n");
            }
            } while(col<1 || col>dim.cols || matrix[0][col-1]==1 );
        }
        

        {// posiziona il gettone //
        for(row=dim.rows-1; row>0; row--){
            if(matrix[row][col] == 0){
                matrix[row][col] = turno;
                break;
            }
        }
        for(int i=1; i<dim.rows; i++){
            for(int j=1; j<dim.cols; j++){
                if(semctl(sem_id,1,GETVAL)==1)
                    printf("[ %c ]", dim.sym2 );
                else if(matrix[i][j] == -turno)
                    printf("[ %c ]", dim.sym1 );
                else
                    printf("[   ]");
            }
            printf("\n");
        }
        }
        
        int size = sizeof(struct player)-sizeof(long);
        if(msgsnd(msq_id,&player,size,0)==-1){
            err_exit("[ERROR] msgsnd");
        }
        // 0,1,1,1,0
        // 0,0,1,1,0
        dec_sem(sem_id,1);
        // 0,1,1,1,0

    } while(semctl(sem_id,3,GETVAL)==0);
    // ... 1

    {// FACCIO IL DETACH DELLA MEMORIA CONDIVISA // 
    dt_shm(shm_ptr);
    }


    return 0;
}

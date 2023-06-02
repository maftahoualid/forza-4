
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

    {
        check_client_args(argc,argv);
    } // args

    printf("\n\n\n\nINIZIO\n");

    int shm_key, *shm_ptr, shm_id;
    int sem_key, sem_id, sem_num = 10;
    int msq_key, msq_id;

    int row=0, col=0;

    struct matrix_dim dim; // STRUTTURA PER SALVARE IL MESSAGGIO //
    int (*matrix)[dim.cols];

    struct player player;
    char name[50];

    struct turno turno;

    struct sembuf sops; // sem_num, sem_op, flg
    unsigned short sem_values[sem_num];
    union semun arg;
    arg.array = sem_values;

    int size=0;


    {
        printf("\n---- creazione semafori ----\n");
        sem_key = ftok("../.",'b'); // key semafori
        printf("sem_key : %d\n", sem_key);
        sem_id = get_sem(sem_key,sem_num); // get sem
        printf("sem_id : %d\n", sem_id);
        printf("---- creazione semafori ----\n");
    } // sem

    {
        semctl(sem_id, 0, GETALL, arg);
        printf("\n---- semafori ----\n");
        for (int i = 0; i < sem_num; i++) {
            printf("SEM %d: [%d]\n", i, sem_values[i]);
        };
        printf("---- semafori ----\n");
    } // stampa set di semafori

    {
        sops = (struct sembuf) {0, +1, 0};
        if (semop(sem_id, &sops, 1)) {
            err_exit("[ERROR] semop");
        };
    } // sem[0]++

    {
        printf("\n---- creazione coda di messaggi ----\n");
        msq_key = ftok("../.", 'c');
        printf("msq_key : %d\n", msq_key);
        msq_id = get_msq(msq_key);
        printf("msq_id : %d\n",msq_id);
        printf("---- creazione coda di messaggi ----\n");
    } // msq

    {
        printf("\n---- ricezione messaggio ----\n");
        size = sizeof(struct matrix_dim)-sizeof(long);
        if(msgrcv(msq_id,&dim,size,(long)1,0)==-1){
            err_exit("[ERROR] msgsnd");
        }
        printf("[%d,%d] [%c,%c]\n",dim.rows,dim.cols,dim.sym1,dim.sym2);
        printf("pid: %d, %s tocca a me\n", getpid(), (dim.tocca_a_me==1)?"":"NON");
        printf("---- ricezione messaggio ----\n");
    } // ricevo i messaggi (matrix_dim dim1 e matrix_dim dim2)

    {
        printf("\n---- creazione memoria condivisa ----\n");
        shm_key = ftok("../.",'a');
        printf("shm_key : %d\n", shm_key);
        shm_id = get_shm(shm_key, sizeof(int[dim.rows][dim.cols]));
        printf("shm_id : %d\n", shm_id);
        shm_ptr = at_shm(shm_id);
        printf("shm_ptr : %p\n",shm_ptr);
        matrix = (void*)shm_ptr;
        printf("matrix : %p\n",matrix);
        printf("---- creazione memoria condivisa ----\n");

    } // shm

    do{

        {
            size = sizeof(struct turno)-sizeof(long);
            if(msgrcv(msq_id,&turno,size,(long)3,0)==-1){
                err_exit("[ERROR] msgsnd");
            }
        } // ricevo tocca a me

        printf("turno: %d", turno.turno);

        if(dim.tocca_a_me==-1){ {
                sops = (struct sembuf) {1, -1, 0};
                if (semop(sem_id, &sops, 1)) {
                    err_exit("[ERROR] semop");
                };
            } // sem[1]--
        } // mi blocco se non tocca a me

        { do {
            printf("\nInserisci una colonna (tra 0 e %d): ",dim.cols-1);
            // SALVO LA MOSSA //
            scanf("%d", &col);
            if(*(shm_ptr+col-1)==1){
                printf("La colonna è piena!\n");
            }
            } while(col<0 || col>dim.cols-1 || *(shm_ptr+col-1)==1 );
        } // inserisci colonna


        { for (row = dim.rows - 1; row >= 0; row--) {
                if (*(shm_ptr+row*dim.cols+col) == 0) {
                    *(shm_ptr+row*dim.cols+col)  = getpid();
                    break;
                }
            }
        } // faccio cadere il gettone

        { for(int i=0; i<dim.rows; i++){
                for(int j=0; j<dim.cols; j++){
                    if( *(shm_ptr+i*dim.cols+j) ==0 )
                        printf("[   ]");
                    else if( *(shm_ptr+i*dim.cols+j) == getpid())
                        printf("[ %c ]", dim.sym1 );
                    else
                        printf("[ %c ]", dim.sym2 );
                }
                printf("\n");
            }
        } // stampo la matrice

        print_matrix(shm_ptr,dim.rows,dim.cols);

        {
            player = (struct player) {(long) 2, dim.sym1, *argv[1]};
            size = sizeof(struct player) - sizeof(long);
            if (msgsnd(msq_id, &player, size, 0) == -1) {
                err_exit("[ERROR] msgsnd");
            }
        } // invio messaggio player

        {
            sops = (struct sembuf) {0, +1, 0};
            if (semop(sem_id, &sops, 1)) {
                err_exit("[ERROR] semop");
            };
        } // sem[0]++

    } while(turno.win==0);

    { dt_shm(shm_ptr); }


    return 0;
}

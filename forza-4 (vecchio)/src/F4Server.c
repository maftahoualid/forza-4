
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

    {
        check_server_args(argc, argv);
    } // args

    printf("\n\n\n\nINIZIO\n");

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    char sym1 = argv[3][0];
    char sym2 = argv[4][0];

    int shm_key, *shm_ptr, shm_id; // shared memory
    int sem_key, sem_id, sem_num = 10; // set di semafori
    int msq_key, msq_id; // coda di messaggi

    struct player player; // mtype, sym, name[50]
    struct sembuf sops; // sem_num, sem_op, flg
    struct turno turno;

    int size = 0;
    int win = 0;
    int isturno = 1;

    unsigned short sem_values[sem_num];
    union semun arg;
    arg.array = sem_values;

    {
        signal(SIGINT, handle_sigint);
    } // sig

    {
        printf("\n---- creazione semafori ----\n");
        sem_key = ftok("../.", 'b'); // key semafori
        printf("sem_key : %d\n", sem_key);
        sem_id = create_sem(sem_key, sem_num); // crea set semafori
        printf("sem_id : %d\n", sem_id);
        init_sem(sem_id); // init set semafori
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
        printf("\n---- creazione coda di messaggi ----\n");
        msq_key = ftok("../.", 'c'); // key coda msg
        printf("msq_key : %d\n", msq_key);
        msq_id = create_msq(msq_key); // crea coda msg
        printf("msq_id : %d\n",msq_id);
        printf("---- creazione coda di messaggi ----\n");
    } // msq

    {
        printf("\n---- creazione memoria condivisa ----\n");
        shm_key = ftok("../.", 'a'); // key shm
        printf("shm_key : %d\n", shm_key);
        shm_id = create_shm(shm_key, sizeof(int[rows][cols])); // crea shm
        printf("shm_id : %d\n", shm_id);
        shm_ptr = at_shm(shm_id); // attach shm
        printf("shm_ptr : %p\n",shm_ptr);
        printf("---- creazione memoria condivisa ----\n");
    } // shm

    {
        init_matrix(shm_ptr, rows, cols); // init shm
        printf("\n---- matrice ----\n");
        print_matrix(shm_ptr, rows, cols); // print matrix
        printf("---- matrice ----\n");
    } // matrix

    {
        sops = (struct sembuf) {0, -1, 0};
        if (semop(sem_id, &sops, 1)) {
            err_exit("[ERROR] semop");
        };
    } // sem[0]-- (server:[0->x], client1:[0->1], server:[1->0])
    printf("\nclient 1 ok\n");

    {
        struct matrix_dim dim1 = {(long) 1, rows, cols, sym1, sym2, isturno};
        size = sizeof(struct matrix_dim) - sizeof(long);
        if (msgsnd(msq_id, &dim1, size, 0) == -1) {
            err_exit("[ERROR] msgsnd");
        }
        turno = (struct turno){(long) 3, isturno, 0 };
        size = sizeof(struct turno) - sizeof(long);
        if (msgsnd(msq_id, &turno, size, 0) == -1) {
            err_exit("[ERROR] msgsnd");
        }
    } // invia primo messaggio (matrix_dim dim1)

    {
        sops = (struct sembuf) {0, -1, 0};
        if (semop(sem_id, &sops, 1)) {
            err_exit("[ERROR] semop");
        };
    } // sem[0]-- (server[0->x], client2:[0->1], server:[1->0])
    printf("client 2 ok\n");

    {
        struct matrix_dim dim2 = {(long) 1, rows, cols, sym2, sym1, -isturno};
        size = sizeof(struct matrix_dim) - sizeof(long);
        if (msgsnd(msq_id, &dim2, size, 0) == -1) {
            err_exit("[ERROR] msgsnd");
        }
        {
            sops = (struct sembuf) {1, +1, 0};
            if (semop(sem_id, &sops, 1)) {
                err_exit("[ERROR] semop");
            };
        } // sem[1]++
        turno = (struct turno){(long) 3, -isturno, 0 };
        size = sizeof(struct turno) - sizeof(long);
        if (msgsnd(msq_id, &turno, size, 0) == -1) {
            err_exit("[ERROR] msgsnd");
        }
    } // invia secondo messaggio (matrix_dim dim2)
    printf("OK");
    do {

        {
            sops = (struct sembuf) {0, -1, 0};
            if (semop(sem_id, &sops, 1)) {
            err_exit("[ERROR] semop");
        };
        } // sem[0]--

        {
            size = sizeof(struct player) - sizeof(long);
            if (msgrcv(msq_id, &player, size, (long) 2, 0) == -1) {
                err_exit("[ERROR] msgrcv");
            }
        } // ricevi la mossa

        win = check_winner(shm_ptr,rows,cols);
        printf("win: %d\n",win);
        if(win==1){
            printf("Player %s, ha vinto!\n", player.name);
            turno.win=1;
            turno = (struct turno){(long) 3, isturno, 1 };
            size = sizeof(struct turno) - sizeof(long);
            if (msgsnd(msq_id, &turno, size, 0) == -1) {
                err_exit("[ERROR] msgsnd");
            }
            exit(0);
        } else {
            printf("Player %s, non ha vinto\n",player.name);
            isturno = -isturno;
            turno = (struct turno){(long) 3, isturno, 0 };
            size = sizeof(struct turno) - sizeof(long);
            if (msgsnd(msq_id, &turno, size, 0) == -1) {
                err_exit("[ERROR] msgsnd");
            }



        }
    } while(win==0);


    dt_shm(shm_ptr);
    rm_shm(shm_id);
    return 0;
}

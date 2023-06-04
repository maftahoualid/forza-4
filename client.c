#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

void err_exit(char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

void usage(){
    printf("[NOTE] uso: F4Server <righe <colonne> <simbolo1> <simbolo2>\n");
}

struct matrix {
    long mtype;
    int rows;
    int cols;
};

struct player_sym {
    long mtype;
    char sym;
};

struct move {
    long mtype;
    char sym;
    int row;
    int col;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

struct win {
    long mtype;
    int value;
    int pid;
};

int main(int argc, char** argv) { //

    {
        if (argc != 2) {
            usage();
            err_exit("[ERROR]: Mi hai passato un numero errato di argomenti.\n");
        } else if (strcmp(argv[1], "*") == 0) {
            printf("(opz) <Notifica il server di duplicarsi e lanciare una copia automatica del client>");
        } else if (strlen(argv[1]) > 50) {
            err_exit("[ERROR]: Il nome del giocatore è troppo lungo!");
        }
    } // check client args

    char *name = argv[1];
    printf("name: %s\n",name);

    printf("\n\n\n\nINIZIO\n");

    int shm_key, shm_id;
    char *shm_ptr;
    int sem_key, sem_id, sem_num = 10;
    int msq_key, msq_id;
    struct matrix matrice = {0,0,0};
    struct player_sym simbolo_giocatore;
    struct win vincita;
    char* matrix;
    struct move mossa = (struct move) {(long) 3};


    int sem_ctl;
    union semun arg;
    struct sembuf sops;
    unsigned short *arr;

    int size;
    int rows;
    int cols;

    int col;

    {
        sem_key = ftok("../.", 'a');
        printf("sem_key : %d\n",sem_key);
    } // sem_key = ftok

    {
        errno = 0;
        sem_id = semget(sem_key, sem_num, 0777);
        printf("sem_id : %d\n",sem_id);
        if (sem_id == -1) {
            if (errno == EEXIST) {
                err_exit("SEMGET : EEXIST");
            } else {
                printf("SEMGET");
            }
        }
    } // sem_id = semget

    /*{
        errno = 0;
        sem_ctl = semctl(sem_id, 0, GETALL, arg);
        if (sem_ctl == -1) {
            err_exit("SEMCTL GETALL\n");
        }
        arr = arg.array;
    } // semctl GETALL*/ // semctl GETALL

    /*{
        for (int i = 0; i < sem_num; i++)
            printf("SEM %d: [%d]\n", i, arr[i]);
    } // print sem values*/ // print sem values

    {
        sops = (struct sembuf) {0, +1, 0};
        printf("inc(connessione client)\n");
        if (semop(sem_id, &sops, 1)) {
            err_exit("[ERROR] semop");
        };
    } // signal(connessione_client)

    {
        msq_key = ftok("../.", 'b');
        printf("msq_key : %d\n",msq_key);
    } // msq_key = ftok b

    {
        errno = 0;
        msq_id = msgget(msq_key, 0666);
        printf("msq_id : %d\n",msq_id);
        if (msq_id == -1) {
            err_exit("[ERROR] msgget");
        }
    } // msq_id = msgget

    {
        size = sizeof(struct matrix) - sizeof(long);
        if (msgrcv(msq_id, &matrice, size, (long) 1, 0) == -1) {
            err_exit("[ERROR] msgrcv");
        }
        printf("receive: (mtype: %ld rows: %d cols: %d)\n",matrice.mtype,matrice.rows,matrice.cols);
        rows = matrice.rows;
        cols = matrice.cols;
    } // msgrcv (matrix)

    {
        size = sizeof(struct player_sym) - sizeof(long);
        if (msgrcv(msq_id, &simbolo_giocatore, size, (long) 2, 0) == -1) {
            err_exit("[ERROR] msgsnd");
        }
        printf("receive: (mtype: %ld sym: %c)\n",simbolo_giocatore.mtype,simbolo_giocatore.sym);
    } // msgrcv (player_sym)

    {
        shm_key = ftok("../.", 'c');
        printf("shm_key: %d\n",shm_key);
    } // shm_key = ftok c

    {
        shm_id = shmget(shm_key, sizeof(char) * rows * cols,0777);
        printf("shm_id: %d\n",shm_id);
        if (shm_id == -1) {
            err_exit("[SHMGET]\n");
        }
    } // shm_id = shmget

    {
        shm_ptr = (char *) shmat(shm_id, NULL, 0);
        printf("shm_ptr: %p\n",shm_ptr);
        if (shm_ptr == (void *)-1)
            err_exit("[SHMAT]\n");
        matrix = shm_ptr;
    } // shm_ptr = shmat

    {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("[%c]", (*(matrix+ ((i*cols)+j) )) );
            }
            printf("\n");
        }
    } // print matrix

    do { //
        {
            sops = (struct sembuf) {2, -1, 0};
            if (semop(sem_id, &sops, 1)) {
                err_exit("[SEMOP]");
            };
            printf("dec(turno)\n");
        } // sem[2]-- blocco client

        {
            do {
                printf("\nInserisci una colonna (tra 1 e %d): ",cols);
                // SALVO LA MOSSA //
                scanf("%d", &col);
                if(*(matrix+col-1)!=' '){
                    printf("La colonna è piena!\n");
                }
            } while(col<1 || col>cols || *(matrix+col-1)!=' ' );
        } // inserisci colonna

        { for (int row = rows-1; row >= 0; row--) {
                if (*(matrix+row*cols+col) == ' ') {
                    *(matrix+row*cols+col)  = simbolo_giocatore.sym;
                    mossa.sym=simbolo_giocatore.sym;
                    printf("sym: %c\n",mossa.sym);
                    mossa.row=row;
                    printf("row: %d\n",mossa.row);
                    mossa.col=col;
                    printf("col: %d\n",mossa.col);
                    break;
                }
            }
        } // faccio cadere il gettone

        {
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    printf("[%c]", (*(matrix+ ((i*cols)+j) )) );
                }
                printf("\n");
            }
        } // print matrix

        {
            size = sizeof(struct move) - sizeof(long);
            if (msgsnd(msq_id, &mossa, size, 0) == -1) {
                err_exit("[MSGSND]");
            }
        } // msgsnd (move)

        {
            size = sizeof(struct win) - sizeof(long);
            if (msgrcv(msq_id, &vincita, size, (long) 4, 0) == -1) {
                err_exit("[ERROR] msgsnd");
            }
        } // msgrcv (win)

        {
            sops = (struct sembuf) {2, +1, 0};
            if (semop(sem_id, &sops, 1)) {
                err_exit("[SEMOP]");
            };
            printf("inc(turno)\n");
        } // sem[2]++ sblocco client
    } while (vincita.value==0);

    if(vincita.value==0){
        if(vincita.pid==getpid()){
            printf("Hai vinto!\n");
        } else {
            printf("Hai perso!\n");
        }
    } else if(vincita.value==-1){
        printf("Hai pareggiato!\n");
    }

    shmdt(shm_ptr);
    matrix = NULL;
    return 0;
}
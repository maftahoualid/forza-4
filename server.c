#include "base.h"
#include "shared_functions.h"
#include "server_functions.h"

int main(int argc, char** argv) { //
    {
        if (argc != 5) {
            usage();
            err_exit("[ERROR]: Mi hai passato un numero errato di argomenti.\n");
        } else if (atoi(argv[1]) < 5) {
            usage();
            err_exit("[ERROR]: La matrice non può avere meno di 5 righe.\n");
        } else if (atoi(argv[2]) < 5) {
            usage();
            err_exit("[ERROR]: La matrice non può avere meno di 5 colonne.\n");
        } else if (strlen(argv[3]) != sizeof(char) || strlen(argv[4]) != sizeof(char)) {
            usage();
            err_exit("[ERROR]: i simboli devono essere lunghi 1 carattere ciascuno.\n");
        } else if (strcmp(argv[3], argv[4]) == 0) {
            err_exit("[ERROR]: i simboli devono essere diversi.\n");
        }
    } // check server args

    int rows = atoi(argv[1]);
    //printf("rows: %d\n",rows);
    int cols = atoi(argv[2]);
    //printf("cols: %d\n",cols);
    char sym1 = argv[3][0];
    //printf("sym1: %c\n",sym1);
    char sym2 = argv[4][0];
    //printf("sym2: %c\n",sym2);

    // printf("\n\n\n\nINIZIO\n");

    struct matrix matrice;
    struct player_sym simbolo_giocatore;
    struct move mossa;
    struct win vincita;
    char *matrix;

    int sem_ctl;
    union semun arg;
    struct sembuf sops;
    unsigned short arr[10] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0};

    int size;
    int win;

    
    signal(SIGINT, handle_sigint );
    // printf("sigint handler set\n");
    // sig

    {
        sem_key = ftok("../.", 'a');
        // printf("sem_key: %d\n",sem_key);
    } // sem_key = ftok a

    {
        errno = 0;
        sem_id = semget(sem_key, sem_num, IPC_CREAT | IPC_EXCL | 0777);
        // printf("sem_id : %d\n",sem_id);
        if (sem_id == -1) {
            if (errno == EEXIST) {
                err_exit("SEMGET : EEXIST");
            } else {
                printf("SEMGET");
            }
        }
        sem_ok=1;
    } // sem_id = semget

    {
        errno = 0;
        arg.array = arr;
        sem_ctl = semctl(sem_id, 10, SETALL, arg);
        if (sem_ctl == -1) {
            err_exit("SEMCTL SETALL\n");
        }
    } // semctl SETALL

    {
        errno = 0;
        arg.array = arr;
        sem_ctl = semctl(sem_id, 0, GETALL, arg);
        if (sem_ctl == -1) {
            err_exit("SEMCTL GETALL\n");
        }
    } // semctl GETALL

    {
        for (int i = 0; i < sem_num; i++) {
            // printf("SEM %d: [%d]\n", i, arr[i]);
        }
    } // print sem values

    {
        msq_key = ftok("../.", 'b');
        // printf("msq_key : %d\n",msq_key);
    } // msq_key = ftok b

    {
        errno = 0;
        msq_id = msgget(msq_key, IPC_CREAT | IPC_EXCL | 0666);
        // printf("msq_id : %d\n",msq_id);
        if (msq_id == -1) {
            if (errno == EEXIST) { err_exit("[ERROR]: coda di messaggi già esistente.\n"); }
            err_exit("[ERROR] msgget");
        }
        msq_ok=1;
    } // msq_id = msgget

    {
        shm_key = ftok("../.", 'c');
        // printf("shm_key: %d\n",shm_key);
    } // shm_key = ftok c

    {
        shm_id = shmget(shm_key, sizeof(char) * rows * cols, IPC_CREAT | IPC_EXCL | 0777);
        // printf("shm_id: %d\n",shm_id);
        if (shm_id == -1) {
            if (errno == EEXIST) { err_exit("[SHMGET : EEXIST]\n"); }
            err_exit("[SHMGET]\n");
        }
        shm_get_ok=1;
    } // shm_id = shmget

    {
        shm_ptr = (char *) shmat(shm_id, NULL, 0);
        // printf("shm_ptr: %p\n",shm_ptr);
        if (shm_ptr == (void *) -1)
            err_exit("[SHMAT]\n");
        matrix = shm_ptr;
        shm_at_ok=1;
    } // shm_ptr = shmat
    char (*mat)[cols] = (void*) matrix;

    {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                mat[i][j] = ' ';
            }
        }
    } // init shm

    /*{
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("[%c]", mat[i][j]);
            }
            printf("\n");
        }
    } */ // print matrix

    {
        sops = (struct sembuf) {0, -1, 0};
        // printf("dec(connessione client)\n");
        if (semop(sem_id, &sops, 1)) {
            err_exit("[SEMOP]");
        };
        // printf("client 1 ok\n");
    } // wait(connessione_client1) [0]--

    {
        matrice = (struct matrix) {(long) 1, rows, cols};
        // printf("send: (mtype: %ld rows: %d, cols %d)\n",matrice.mtype,matrice.rows,matrice.cols);
        size = sizeof(struct matrix) - sizeof(long);
        if (msgsnd(msq_id, &matrice, size, 0) == -1) {
            err_exit("[MSGSND]");
        }
    } // msgsnd (matrix)

    {
        simbolo_giocatore = (struct player_sym) {(long) 2, sym1};
        // printf("send: (mtype: %ld sym1: %c)\n",simbolo_giocatore.mtype,simbolo_giocatore.sym);
        size = sizeof(struct player_sym) - sizeof(long);
        if (msgsnd(msq_id, &simbolo_giocatore, size, 0) == -1) {
            err_exit("[MSGSND]");
        }
    } // msgsnd (player_sym)

    {
        sops = (struct sembuf) {0, -1, 0};
        // printf("dec(connessione client)\n");
        if (semop(sem_id, &sops, 1)) {
            err_exit("[SEMOP]");
        };
        // printf("\nclient 2 ok\n");
    } // wait(connessione_client2) [0]--

    {
        matrice = (struct matrix) {(long) 1, rows, cols};
        // printf("send: (mtype: %ld rows: %d, cols %d)\n",matrice.mtype,matrice.rows,matrice.cols);
        size = sizeof(struct matrix) - sizeof(long);
        if (msgsnd(msq_id, &matrice, size, 0) == -1) {
            err_exit("[MSGSND]");
        }
    } // msgsnd (matrix)

    {
        simbolo_giocatore = (struct player_sym) {(long) 2, sym2};
        // printf("send: (mtype: %ld sym1: %c)\n",simbolo_giocatore.mtype,simbolo_giocatore.sym);
        size = sizeof(struct player_sym) - sizeof(long);
        if (msgsnd(msq_id, &simbolo_giocatore, size, 0) == -1) {
            err_exit("[MSGSND]");
        }
    } // msgsnd (player_sym)

    {
        do { //
            // printf("waiting to receive move\n");
            {
                size = sizeof(struct move) - sizeof(long);
                if (msgrcv(msq_id, &mossa, size, (long) 3, 0) == -1) {
                    err_exit("[ERROR] msgsnd");
                }
                // printf("receive: (mtype: %ld sym: %c pid: %d row: %d col: %d)\n",mossa.mtype,mossa.sym,mossa.pid,mossa.row,mossa.col);
            } // msgrcv (move)

            win = check_win(matrix, rows, cols);
            // printf("win : %d\n",win);

            {
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        printf("[%c]", mat[i][j] );
                    }
                    printf("\n");
                }
            } // print matrix

            if (win == 1) { //
                printf("Vincita\n");
                {
                    vincita = (struct win) {(long) 4, 1, mossa.pid};
                    size = sizeof(struct win) - sizeof(long);
                    if (msgsnd(msq_id, &vincita, size, 0) == -1) {
                        err_exit("[MSGSND]");
                    }
                    //printf("send: (mtype: %ld value: %d pid: %d)\n",vincita.mtype,vincita.value,vincita.pid);
                } // msgsnd (win)
                {
                    sops = (struct sembuf) {2, -1, 0};
                    if (semop(sem_id, &sops, 1)) {
                        err_exit("[SEMOP]");
                    };
                    //printf("dec(server)\n");
                } // sem[2]-- blocco server
                {
                    size = sizeof(struct move) - sizeof(long);
                    if (msgrcv(msq_id, &mossa, size, (long) 3, 0) == -1) {
                        err_exit("[ERROR] msgsnd");
                    }
                    //printf("receive: (mtype: %ld sym: %c pid: %d row: %d col: %d)\n",mossa.mtype,mossa.sym,mossa.pid,mossa.row,mossa.col);
                } // msgrcv (move)
                {
                    vincita = (struct win) {(long) 4, 1, 0};
                    size = sizeof(struct win) - sizeof(long);
                    if (msgsnd(msq_id, &vincita, size, 0) == -1) {
                        err_exit("[MSGSND]");
                    }
                } // msgsnd (win)
                {
                    sops = (struct sembuf) {2, -1, 0};
                    if (semop(sem_id, &sops, 1)) {
                        err_exit("[SEMOP]");
                    };
                    //printf("dec(server)\n");
                } // sem[2]-- blocco server
                close_server();
            } else if (win == 0) {
                printf("La partita continua\n");
                {
                    vincita = (struct win) {(long) 4, 0, mossa.pid};
                    size = sizeof(struct win) - sizeof(long);
                    if (msgsnd(msq_id, &vincita, size, 0) == -1) {
                        err_exit("[MSGSND]");
                    }
                } // msgsnd (win)
            } else if (win == -1) {
                printf("Pareggio\n");
                {
                    vincita = (struct win) {(long) 4, -2, mossa.pid};
                    size = sizeof(struct win) - sizeof(long);
                    if (msgsnd(msq_id, &vincita, size, 0) == -1) {
                        err_exit("[MSGSND]");
                    }
                } // msgsnd (win)
                {
                    sops = (struct sembuf) {2, -1, 0};
                    if (semop(sem_id, &sops, 1)) {
                        err_exit("[SEMOP]");
                    };
                    //printf("dec(server)\n");
                } // sem[2]-- blocco server
                {
                    size = sizeof(struct move) - sizeof(long);
                    if (msgrcv(msq_id, &mossa, size, (long) 3, 0) == -1) {
                        err_exit("[ERROR] msgsnd");
                    }
                    //printf("receive: (mtype: %ld sym: %c pid: %d row: %d col: %d)\n",mossa.mtype,mossa.sym,mossa.pid,mossa.row,mossa.col);
                } // msgrcv (move)
                {
                    vincita = (struct win) {(long) 4, -2, mossa.pid};
                    size = sizeof(struct win) - sizeof(long);
                    if (msgsnd(msq_id, &vincita, size, 0) == -1) {
                        err_exit("[MSGSND]");
                    }
                } // msgsnd (win)
                {
                    sops = (struct sembuf) {2, -1, 0};
                    if (semop(sem_id, &sops, 1)) {
                        err_exit("[SEMOP]");
                    };
                    // printf("dec(server)\n");
                } // sem[2]-- blocco server
                close_server();
            }
        } while (win==0);
    } // play
    return 0;
}
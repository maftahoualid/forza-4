#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

int sem_ok=0;
int msq_ok=0;
int shm_get_ok=0;
int shm_at_ok=0;

int shm_key, shm_id;
char *shm_ptr;
int sem_key, sem_id, sem_num = 10;
int msq_key, msq_id;

void close_server(){
    printf("[NOTE]: .... closing server ....\n");
    if(sem_ok){
        semctl(sem_id,IPC_RMID,0);
    }
    if(msq_ok){
        msgctl(msq_id, IPC_RMID, NULL);
    }
    if(shm_at_ok){
        shmdt(shm_ptr);
    }
    if(shm_get_ok){
        shmctl(shm_id,IPC_RMID,NULL);
    }
};

void err_exit(char *msg){
    perror(msg);
    close_server();
    exit(EXIT_FAILURE);
}

void usage(){
    printf("[NOTE] uso: F4Server <righe <colonne> <simbolo1> <simbolo2>\n");
}

int sigint_count = 0;
void handle_sigalrm(){
    sigint_count=0;
}

void handle_sigint(){
    signal(SIGALRM, handle_sigalrm); //GESTISCO IL TIMER PER IL SECONDO CTRL-C
    if(sigint_count==0){
        printf("[WARNING]: CTRL-C 1: premi un'altra volta per terminare il server\n");
        sigint_count++;
        alarm(10);
    } else {
        printf("[NOTE]: CTRL-C 2: termino il server...\n");
        close_server();
    }
}

int check_win(char* matrix,int rows,int cols){
    int i,j;

    // ROW WIN
    for (i=0;i<rows;i++){
        for(j=0; j<cols-4;j++){
            char elem = *(matrix+i*cols+j);
            int row_win = ((elem!=' ')&&
                    (elem==*(matrix+i*cols+j+1))&&
                    (elem==*(matrix+i*cols+j)+2)&&
                    (elem==*(matrix+i*cols+j)+3) ? 1 : 0);
            if(row_win==1) return 1;
        }
    }

    // COL WIN
    for (i=0;i<rows-4;i++){
        for(j=0; j<cols;j++){
            char elem = *(matrix+i*cols+j);
            int row_win = ((elem!=' ')&&
                    (elem==*(matrix+i*(cols+1)+j))&&
                    (elem==*(matrix+i*(cols+2)+j))&&
                    (elem==*(matrix+i*(cols+3)+j)) ? 1 : 0);
            if(row_win==1) return 1;
        }
    }

    for (i=0;i<rows;i++){
        for(j=0; j<cols;j++){
            if(*(matrix+i*cols+j) == ' '){
                return 0;
            }
        }
    }

    return -1;
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
    int pid;
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
    printf("rows: %d\n",rows);
    int cols = atoi(argv[2]);
    printf("cols: %d\n",cols);
    char sym1 = argv[3][0];
    printf("sym1: %c\n",sym1);
    char sym2 = argv[4][0];
    printf("sym2: %c\n",sym2);

    printf("\n\n\n\nINIZIO\n");

    struct matrix matrice;
    struct player_sym simbolo_giocatore;
    struct move mossa;
    struct win vincita;
    char *matrix;

    int sem_ctl;
    union semun arg;
    struct sembuf sops;
    unsigned short arr[10] = {0, 0, 1, 0, 0, 0, 0, 0, 0, 0};

    int size;
    int win;

    {
        signal(SIGINT, handle_sigint);
        printf("sigint handler set\n");
    } // sig

    {
        sem_key = ftok("../.", 'a');
        printf("sem_key: %d\n",sem_key);
    } // sem_key = ftok a

    {
        errno = 0;
        sem_id = semget(sem_key, sem_num, IPC_CREAT | IPC_EXCL | 0777);
        printf("sem_id : %d\n",sem_id);
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
        for (int i = 0; i < sem_num; i++)
            printf("SEM %d: [%d]\n", i, arr[i]);
    } // print sem values

    {
        msq_key = ftok("../.", 'b');
        printf("msq_key : %d\n",msq_key);
    } // msq_key = ftok b

    {
        errno = 0;
        msq_id = msgget(msq_key, IPC_CREAT | IPC_EXCL | 0666);
        printf("msq_id : %d\n",msq_id);
        if (msq_id == -1) {
            if (errno == EEXIST) { err_exit("[ERROR]: coda di messaggi già esistente.\n"); }
            err_exit("[ERROR] msgget");
        }
        msq_ok=1;
    } // msq_id = msgget

    {
        shm_key = ftok("../.", 'c');
        printf("shm_key: %d\n",shm_key);
    } // shm_key = ftok c

    {
        shm_id = shmget(shm_key, sizeof(char) * rows * cols, IPC_CREAT | IPC_EXCL | 0777);
        printf("shm_id: %d\n",shm_id);
        if (shm_id == -1) {
            if (errno == EEXIST) { err_exit("[SHMGET : EEXIST]\n"); }
            err_exit("[SHMGET]\n");
        }
        shm_get_ok=1;
    } // shm_id = shmget

    {
        shm_ptr = (char *) shmat(shm_id, NULL, 0);
        printf("shm_ptr: %p\n",shm_ptr);
        if (shm_ptr == (void *) -1)
            err_exit("[SHMAT]\n");
        matrix = shm_ptr;
        shm_at_ok=1;
    } // shm_ptr = shmat

    {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                (*(matrix + ((i * cols) + j))) = ' ';
            }
        }
    } // init shm

    {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("[%c]", (*(matrix + ((i * cols) + j))));
            }
            printf("\n");
        }
    } // print matrix

    {
        sops = (struct sembuf) {0, -1, 0};
        printf("dec(connessione client)\n");
        if (semop(sem_id, &sops, 1)) {
            err_exit("[SEMOP]");
        };
        printf("client 1 ok\n");
    } // wait(connessione_client1)

    {
        matrice = (struct matrix) {(long) 1, rows, cols};
        printf("send: (mtype: %ld rows: %d, cols %d)\n",matrice.mtype,matrice.rows,matrice.cols);
        size = sizeof(struct matrix) - sizeof(long);
        if (msgsnd(msq_id, &matrice, size, 0) == -1) {
            err_exit("[MSGSND]");
        }
    } // msgsnd (matrix)

    {
        simbolo_giocatore = (struct player_sym) {(long) 2, sym1};
        printf("send: (mtype: %ld sym1: %c)\n",simbolo_giocatore.mtype,simbolo_giocatore.sym);
        size = sizeof(struct player_sym) - sizeof(long);
        if (msgsnd(msq_id, &simbolo_giocatore, size, 0) == -1) {
            err_exit("[MSGSND]");
        }
    } // msgsnd (player_sym)

    {
        sops = (struct sembuf) {0, -1, 0};
        printf("dec(connessione client)\n");
        if (semop(sem_id, &sops, 1)) {
            err_exit("[SEMOP]");
        };
        printf("\nclient 2 ok\n");
    } // wait(connessione_client2)

    {
        matrice = (struct matrix) {(long) 1, rows, cols};
        printf("send: (mtype: %ld rows: %d, cols %d)\n",matrice.mtype,matrice.rows,matrice.cols);
        size = sizeof(struct matrix) - sizeof(long);
        if (msgsnd(msq_id, &matrice, size, 0) == -1) {
            err_exit("[MSGSND]");
        }
    } // msgsnd (matrix)

    {
        simbolo_giocatore = (struct player_sym) {(long) 2, sym2};
        printf("send: (mtype: %ld sym1: %c)\n",simbolo_giocatore.mtype,simbolo_giocatore.sym);
        size = sizeof(struct player_sym) - sizeof(long);
        if (msgsnd(msq_id, &simbolo_giocatore, size, 0) == -1) {
            err_exit("[MSGSND]");
        }
    } // msgsnd (player_sym)

    {
        do {

            {
                size = sizeof(struct move) - sizeof(long);
                if (msgrcv(msq_id, &mossa, size, (long) 3, 0) == -1) {
                    err_exit("[ERROR] msgsnd");
                }
                printf("receive: (mtype: %ld sym: %c pid: %d col: %d)\n",mossa.mtype,mossa.sym,mossa.pid,mossa.col);
            } // msgrcv (move)

            win = check_win(matrix, rows, cols);
            printf("win : %d\n",win);

            if (win == 1) { //
                printf("Vincita\n");
                {
                    vincita = (struct win) {(long) 4, 1, mossa.pid};
                    size = sizeof(struct win) - sizeof(long);
                    if (msgsnd(msq_id, &vincita, size, 0) == -1) {
                        err_exit("[MSGSND]");
                    }
                } // msgsnd (win)
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
                    vincita = (struct win) {(long) 4, -1, mossa.pid};
                    size = sizeof(struct win) - sizeof(long);
                    if (msgsnd(msq_id, &vincita, size, 0) == -1) {
                        err_exit("[MSGSND]");
                    }
                } // msgsnd (win)
                close_server();
            }
        } while (win==0);
    } // play
    return 0;
}
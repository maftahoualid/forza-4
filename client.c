#include "base.h"
#include "shared_functions.h"
#include "client_functions.h"

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
    //printf("name: %s\n",name);

    //printf("\n\n\n\nINIZIO\n");

    int shm_key, shm_id;
    char *shm_ptr;
    int sem_key, sem_id, sem_num = 10;
    int msq_key, msq_id;

    char* matrix;
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
        // printf("sem_key : %d\n",sem_key);
    } // sem_key = ftok

    {
        errno = 0;
        sem_id = semget(sem_key, sem_num, 0777);
        // printf("sem_id : %d\n",sem_id);
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
        // printf("inc(connessione client)\n");
        if (semop(sem_id, &sops, 1)) {
            err_exit("[ERROR] semop");
        };
    } // signal(connessione_client) [0]++

    {
        msq_key = ftok("../.", 'b');
        // printf("msq_key : %d\n",msq_key);
    } // msq_key = ftok b

    {
        errno = 0;
        msq_id = msgget(msq_key, 0666);
        // printf("msq_id : %d\n",msq_id);
        if (msq_id == -1) {
            err_exit("[ERROR] msgget");
        }
    } // msq_id = msgget

    struct matrix matrice = {0,0,0};
    // printf("waiting to receive matrix\n");
    {
        size = sizeof(struct matrix) - sizeof(long);
        if (msgrcv(msq_id, &matrice, size, (long) 1, 0) == -1) {
            err_exit("[ERROR] msgrcv");
        }
        // printf("receive: (mtype: %ld rows: %d cols: %d)\n",matrice.mtype,matrice.rows,matrice.cols);
        rows = matrice.rows;
        cols = matrice.cols;
    } // msgrcv (matrix)

    struct player_sym simbolo_giocatore = {1,' '};
    // printf("waiting to receive player sym\n");
    {
        size = sizeof(struct player_sym) - sizeof(long);
        if (msgrcv(msq_id, &simbolo_giocatore, size, (long) 2, 0) == -1) {
            err_exit("[ERROR] msgsnd");
        }
        // printf("receive: (mtype: %ld sym: %c)\n",simbolo_giocatore.mtype,simbolo_giocatore.sym);
    } // msgrcv (player_sym)

    {
        shm_key = ftok("../.", 'c');
        // printf("shm_key: %d\n",shm_key);
    } // shm_key = ftok c

    {
        shm_id = shmget(shm_key, sizeof(char) * rows * cols,0777);
        // printf("shm_id: %d\n",shm_id);
        if (shm_id == -1) {
            err_exit("[SHMGET]\n");
        }
    } // shm_id = shmget

    {
        shm_ptr = (char *) shmat(shm_id, NULL, 0);
        // printf("shm_ptr: %p\n",shm_ptr);
        if (shm_ptr == (void *)-1)
            err_exit("[SHMAT]\n");
        matrix = shm_ptr;
    } // shm_ptr = shmat
    char (*mat)[cols] = (void*) matrix;

    {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("[%c]", mat[i][j] );
            }
            printf("\n");
        }
    } // print matrix

    struct move mossa = (struct move) {(long) 3,0,' ',0,0};
    struct win vincita = {(long)4,0,0};
    do { //
        if(vincita.value!=0){
            printf("vincita.value: 1");
            break;
        } //

        {
            // printf("dec(turno)\n");
            sops = (struct sembuf) {1, -1, 0};
            if (semop(sem_id, &sops, 1)) {
                err_exit("[SEMOP]");
            };
        } // sem[1]-- blocco client

        { //
            do {
                printf("\nInserisci una colonna (tra 1 e %d): ",cols);
                // SALVO LA MOSSA //
                scanf("%d", &col);
                if(mat[0][col-1]!=' '){
                    printf("La colonna è piena!\n");
                }
            } while(col<1 || col>cols || mat[0][col-1]!=' ' );
        } // inserisci colonna

        { //
            for (int row = rows-1; row >= 0; row--) {
                if (mat[row][col-1] == ' ') {
                    mat[row][col-1]  = simbolo_giocatore.sym;
                    mossa.sym=simbolo_giocatore.sym;
                    // printf("sym: %c\n",mossa.sym);
                    mossa.pid=getpid();
                    // printf("pid: %d\n",mossa.pid);
                    mossa.row=row;
                    // printf("row: %d\n",mossa.row);
                    mossa.col=col;
                    // printf("col: %d\n",mossa.col);
                    break;
                }
            }
        } // faccio cadere il gettone

        {
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    printf("[%c]", mat[i][j] );
                }
                printf("\n");
            }
        } // print matrix

        {
            size = sizeof(struct move) - sizeof(long);
            if (msgsnd(msq_id, &mossa, size, 0) == -1) {
                err_exit("[MSGSND]");
            }
            // printf("send: (mtype: %ld sym: %c pid: %d row: %d col: %d)\n",mossa.mtype,mossa.sym,mossa.pid,mossa.row,mossa.col);
        } // msgsnd (move)

        // printf("waiting to receive win\n");
        {
            size = sizeof(struct win) - sizeof(long);
            if (msgrcv(msq_id, &vincita, size, (long) 4, 0) == -1) {
                err_exit("[ERROR] msgsnd");
            }
            // printf("receive: (mtype: %ld value: %d pid: %d)\n",vincita.mtype,vincita.value,vincita.pid);
        } // msgrcv (win)

        {
            // printf("inc(turno)\n");
            sops = (struct sembuf) {1, +1, 0};
            if (semop(sem_id, &sops, 1)) {
                err_exit("[SEMOP]");
            };
        } // sem[1]++ sblocco client
    } while (vincita.value==0);

    if(vincita.value==1){
        if(vincita.pid==getpid()){
            printf("Hai vinto!\n");
        } else {
            printf("Hai perso!\n");
        }
    } else if(vincita.value==-1){
        printf("Hai pareggiato!\n");
    } else {
        printf("ERRORE");
    }


    shmdt(shm_ptr);
    matrix = NULL;

    {
        // printf("inc(server)\n");
        sops = (struct sembuf) {2, +1, 0};
        if (semop(sem_id, &sops, 1)) {
            err_exit("[SEMOP]");
        };
    } // sem[3]++ sblocco server

    return 0;
}
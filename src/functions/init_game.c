#include "../../headers/base.h"
#include "../../headers/err_exit.h"
#include "../../headers/init_game.h"

void check_args(int argc, char **argv){
    if(argc!=5){
        usage();
        err_exit("[ERROR]: Mi hai passato un numero errato di argomenti.\n");
    } 
    else if(atoi(argv[1])<5){
        usage();
        err_exit("[ERROR]: La matrice non può avere meno di 5 righe.\n");
    } 
    else if(atoi(argv[2])<5){
        usage();
        err_exit("[ERROR]: La matrice non può avere meno di 5 colonne.\n");
    } 
}

int sigint_count = 0;
void handle_sigint(){
    if(sigint_count==0){
        printf("[WARNING]: CTRL-C 1: premi un'altra volta per terminare il server\n");
        sigint_count++;
        alarm(10);
    } else {
        printf("[NOTE]: CTRL-C 2: termino il server...\n");
        close_server();
    }
}

void handle_sigalrm(){
    sigint_count=0;
}

void close_server(){
    printf("[NOTE]: .... closing server .... (da implementare)\n");
    // devo comunicare ai client e ripulire memoria e semafori
    exit(0);
};

void usage(){
    printf("[NOTE] uso: F4Server <righe <colonne> <simbolo1> <simbolo2>\n");
}

void init_matrix(void *shm_ptr, int rows, int cols){
    int (*matrix)[cols] = (void*)shm_ptr;
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            matrix[i][j] = 0;
        }
    }
}

void print_matrix(void *shm_ptr, int rows, int cols){
    int (*matrix)[cols] = (void*)shm_ptr;
    for(int i=1; i<rows; i++){
        for(int j=1; j<cols; j++){
            printf("[%d]", matrix[i][j]);
        }
        printf("\n");
    }
}

int get_shm(int shm_key, int size) {
    errno=0;
    int shm_id = shmget(shm_key, size, IPC_CREAT | IPC_EXCL | 0666 );
    if (shm_id == -1){
        if(errno == EEXIST){ err_exit("[ERROR]: memoria condivisa già esistente.\n"); }
        err_exit("[ERROR]: nella creazione della memoria.\n");
    }
    return shm_id;
}

void *at_shm(int shm_id, int shm_flg) {
    void *shm_ptr = shmat(shm_id, NULL, shm_flg);
    if (shm_ptr == (void *)-1)
        err_exit("[ERRORE]: nell'attach della memoria.\n");
    return shm_ptr;
}

void dt_shm(void *ptr_sh) {
    int shm_dt = shmdt(ptr_sh);
    if (shm_dt == -1)
        err_exit("[ERROR]: shmdt failed.\n");
}

void rm_shm(int shm_id) {
    int shm_ctl = shmctl(shm_id, IPC_RMID, NULL);
    if (shm_ctl == -1)
        err_exit("[ERROR]: shmctl failed.\n");
}

int get_sem(int sem_key, int sem_num){
    errno=0;
    int sem_id = semget(sem_key,sem_num, IPC_CREAT | 0666 );
    if (sem_id==-1){
        //if(errno==EEXIST) { err_exit("ERRORE: set di semafori già esistente");}
        err_exit("[ERROR]: nella creazione del set di semafori.\n");
    }
    return sem_id;
}

void ctl_sem(int sem_id, int sem_num){
    errno=0;
    union semun arg;
    unsigned short arr[sem_num];
    for(int i=0; i<sem_num; i++){ arr[i]=0; }
    arg.array = arr;
    
    int sem_ctl = semctl(sem_id, sem_num, SETALL, arg);
    if (sem_ctl == -1) {
        err_exit("[ERROR]: nell'inizializzazione del semaforo.\n");
    }
}

void wait_sem(int sem_id) {
    struct sembuf sem_op = (struct sembuf){0, -1, 0};
    errno = 0;
    int semop_ret;
    do {
        if(errno==EINTR) { printf("[WARNING]: system call interrotta, riprovo\n"); }
        semop_ret = semop(sem_id, &sem_op, 1);
    } while (semop_ret==-1 && errno==EINTR);
}

void signal_sem(int sem_id){
    struct sembuf sem_op = (struct sembuf){0,1,0};
    int semop_ret;
    errno = 0;
    do {
        if(errno==EINTR) { printf("[WARNING]: system call interrotta, riprovo\n"); }
        semop_ret = semop(sem_id, &sem_op, 1);
    } while (semop_ret==-1 && errno==EINTR);
}

void forza4(int sem_id, int* shm_ptr, int rows, int cols){
    int (*matrix)[cols] = (void*)shm_ptr;
    int player_num = 1;
    int row=0, col=0;
    int winner=0;

    while (winner==0){
        wait_sem(sem_id);
    }

    do {
        if(matrix[0][cols]!=1 || matrix[0][cols]!=0){
            printf("La colonna è piena!\n");
        }
        printf("Inserisci colonna (da 0 a %i): ",cols);
        scanf("%d",&col);
    } while(col<0 || col>cols || matrix[0][cols]!=1 || matrix[0][cols]!=0);

    for(row=rows-1; row>=0; row++){
        if(matrix[row][col] == 0){
            matrix[row][col] = player_num;
            break;
        }
    }

    winner = check_winner();
    if(winner!=0){
        printf("Vince giocatore %d", player_num);
    } else { player_num = (player_num)?2:1; }

    close_server();
}

int check_winner(int* shm_ptr, int rows, int cols){
    int (*matrix)[cols] = (void*)shm_ptr;
    int i,j;

    // ROW WIN
    for (i=0;i<rows;i++){
        for(j=0; j<cols-4;j++){
            int elem = matrix[i][j];
            int row_win = ((elem!=0)&&
                          (elem==matrix[i][j+1])&&
                          (elem==matrix[i][j+2])&&
                          (elem==matrix[i][j+3])) ? 1 : 0;
            if(row_win==1) return elem;
        }
    }
    // COL WIN
    for (i=0;i<rows-4;i++){
        for(j=0; j<cols;j++){
            int elem = matrix[i][j];
            int row_win = ((elem!=0)&&
                          (elem==matrix[i+1][j])&&
                          (elem==matrix[i+2][j])&&
                          (elem==matrix[i+3][j])) ? 1 : 0;
            if(row_win==1) return elem;
        }
    }
    // (DIAGONAL OR) NO WIN
    return 0;
}
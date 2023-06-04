#include "server_functions.h"

int sem_ok=0;
int msq_ok=0;
int shm_get_ok=0;
int shm_at_ok=0;

int shm_key=0, shm_id=0;
char *shm_ptr=NULL;
int sem_key=0, sem_id=0, sem_num = 10;
int msq_key=0, msq_id=0;

int sigint_count = 0;

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

void handle_sigalrm(){
    sigint_count=0;
};

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
};

int check_win(char* matrix,int rows,int cols){
    int i,j;
    char (*mat)[cols] = (void*) matrix;

    // ROW WIN
    for (i=0;i<rows;i++){
        for(j=0; j<cols-3;j++){

//            printf("Controllo vincita per riga: riga %d\n",i);
//            printf("[%c] ", mat[i][j]);
//            printf("[%c] ", mat[i][j+1]);
//            printf("[%c] ", mat[i][j+2]);
//            printf("[%c]\n", mat[i][j+3]);
//            printf("\n");

            char elem = mat[i][j];
            int row_win = ((elem!=' ')&&
                           (elem==mat[i][j+1])&&
                           (elem==mat[i][j+2])&&
                           (elem==mat[i][j+3]) ? 1 : 0);
            if(row_win==1) return 1;
        }
    }

    // COL WIN
    for (i=0;i<rows-3;i++){
        for(j=0; j<cols;j++){

//            printf("Controllo vincita per colonna: colonna %d\n",j);
//            printf("[%c]\n", mat[i][j]);
//            printf("[%c]\n", mat[i+1][j]);
//            printf("[%c]\n", mat[i+2][j]);
//            printf("[%c]\n", mat[i+3][j]);
//            printf("\n");

            char elem = mat[i][j];
            int row_win = ((elem!=' ')&&
                           (elem==mat[i+1][j])&&
                           (elem==mat[i+2][j])&&
                           (elem==mat[i+3][j]) ? 1 : 0);
            if(row_win==1) return 1;
        }
    }

    for (i=0;i<rows;i++){
        for(j=0; j<cols;j++){
            if(mat[i][j] == ' '){
                printf("Matrice non piena\n");
                return 0;
            }
        }
    }

    return -1;
}
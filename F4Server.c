#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

void checkArgs(int argc, char **argv);
void printMatrix(int* shm_ptr, int rows, int cols);
void errExit(char *msg);

// F4Server 6 7 O X
int main(int argc, char **argv) {
    // max 5 parametri
    // min 5 righe
    // min 5 colonne
    checkArgs(argc, argv);

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    
    int *shm_ptr;
    int shm_id;

    int semid;
    
    /* SHMGET */
    errno = 0;
    if(shm_id=shmget(IPC_PRIVATE, sizeof(int[rows][cols]), IPC_CREAT | IPC_EXCL | 0666 ) == -1){ 
            if(errno == EEXIST){ errExit("ERRORE: memoria condivisa già esistente"); }
            errExit("ERRORE nella creazione dela memoria");
    }    

    /* SHMAT */
    if(shm_ptr = shmat(shm_id, NULL, 0) == NULL){
        errExit("ERRORE nell'attach della memoria");
    }

    printMatrix(shm_ptr,rows,cols);

    semid = semget(IPC_PRIVATE,1,IPC_CREAT | IPC_EXCL);



} 

void checkArgs(int argc, char **argv){
    if(argc>5){
        errExit("Mi hai passato troppi argomenti.\n");
    }

    if(atoi(argv[1])<5){
        errExit("La matrice non può avere meno di 5 righe.\n");
    }

    if(atoi(argv[2])<5){
        errExit("La matrice non può avere meno di 5 colonne.\n");
    }

}

void printMatrix(int* shm_ptr, int rows, int cols){
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            printf("[%d]", *(shm_ptr+i*cols+j));
        }
        printf("\n");
    }
}

void errExit(char *msg){
    printf("%s", msg);

    printf("uso: F4Server <righe <colonne> <simbolo1> <simbolo2>\n");
    printf("\t- <righe> non deve essere minore di 5.\n");
    printf("\t- <colonne> non deve essere minore di 5.\n");

    exit(EXIT_FAILURE);
}
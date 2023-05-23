/**
 * Certamente! Di seguito trovi un elenco puntato delle attività da svolgere per implementare il gioco "Forza 4" utilizzando le system call SYSTEMV in ambiente UNIX/LINUX:

1. Implementazione del server (F4Server):
   - Analizzare e interpretare i parametri di linea di comando (dimensioni del campo, gettoni dei giocatori).
   - Creare l'area di memoria condivisa per il campo di gioco.
   - Inizializzare la memoria condivisa con il campo vuoto.
   - Creare e inizializzare i semafori per la sincronizzazione dei giocatori.
   - Gestire gli errori in caso di presenza precedente di campi di gioco (memoria condivisa e semafori).
   - Gestire il comando CTRL-C per terminare correttamente il gioco.
   - Arbitrare la partita, controllando le mosse dei giocatori e verificando le condizioni di vittoria.
   - Notificare ai client se hanno vinto o se la partita è finita in parità.
   - Gestire la terminazione del gioco e rimuovere correttamente le IPC (memoria condivisa e semafori).

2. Implementazione del client (F4Client):
   - Analizzare e interpretare il parametro di linea di comando (nome del giocatore).
   - Connettersi al server e attendere il secondo giocatore.
   - Notificare al giocatore la ricerca di un avversario.
   - Visualizzare la matrice di gioco aggiornata.
   - Richiedere al giocatore la colonna in cui inserire il gettone.
   - Controllare se la colonna è disponibile o piena.
   - Gestire il comando CTRL-C per terminare correttamente il gioco.
   - Notificare al server la vittoria per abbandono in caso di disconnessione del giocatore.

3. Funzionalità aggiuntive:
   - Implementare il timeout per ogni mossa e decidere l'azione da intraprendere al suo scadere.
   - Creare un client automatico che effettua mosse casuali se eseguito con l'apposita opzione.

Ricorda che durante l'implementazione dovrai utilizzare le system call SYSTEMV, come la gestione dei processi figli, la memoria condivisa, i semafori e i segnali tra processi. Assicurati di gestire correttamente gli errori e di seguire le regole del gioco descritte nel testo.

Se hai ulteriori domande o hai bisogno di ulteriori chiarimenti su una specifica parte dell'implementazione, sarò qui per aiutarti.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <fcntl.h>


void check_args(int argc, char **argv);
void init_matrix(int *shm_ptr, int rows, int cols);
void print_matrix(int* shm_ptr, int rows, int cols);
int get_shm(int shm_key, int size);
void *at_shm(int shm_id, int shm_flg);
void dt_shm(void *ptr_sh);
void rm_shm(int shm_id);
int get_sem(int sem_key, int sem_num);
void ctl_sem(int sem_id, int sem_num);
void usage();
void err_exit(char *msg);

union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
};

// F4Server 6 7 O X
int main(int argc, char **argv) {

    /* CONTROLLO VALIDITA' DEI PARAMETRI */
    check_args(argc, argv);
    
    /* DO UN NOME AI PARAMETRI */
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    int shm_key; 
    int *shm_ptr; // PUNTATORE A MEMORIA CONDIVISA
    int shm_id; // ID MEMORIA CONDIVISA

    int sem_key;
    int sem_id; // ID SEMAFORI
    int sem_num = 10; // NUMERO SEMAFORI
    
    shm_key = ftok("../.",'a');
    // CREO LA MEMORIA CONDIVISA //
    shm_id = get_shm(shm_key, sizeof(int[rows][cols]));

    // FACCIO L'ATTACH DELLA MEMORIA //
    shm_ptr = at_shm(shm_id, 0);

    // INIZIALIZZO LA MATRICE //
    init_matrix(shm_ptr,rows,cols);

    // STAMPO LA MATRICE // 
    print_matrix(shm_ptr,rows,cols);

    sem_key = ftok("../.",'b');
    // CREO IL SET DI SEMAFORI //
    sem_id = get_sem(sem_key,sem_num);

    // INIZIALIZZO IL SET DI SEMAFORI //
    ctl_sem(sem_id,sem_num);

    // FACCIO IL DETACH DELLA MEMORIA // 
    dt_shm(shm_ptr);

    // RIMUOVO LA MEMORIA CONDIVISA // 
    rm_shm(shm_id);

    return 0;
} 




void check_args(int argc, char **argv){
    if(argc>5){
        usage();
        err_exit("Mi hai passato troppi argomenti.");
    } 
    else if(atoi(argv[1])<5){
        usage();
        err_exit("La matrice non può avere meno di 5 righe.");
    } 
    else if(atoi(argv[2])<5){
        usage();
        err_exit("La matrice non può avere meno di 5 colonne.");
    } 
}

void init_matrix(int *shm_ptr, int rows, int cols){
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            *(shm_ptr+i*cols+j) = 0;
        }
    }
}

void print_matrix(int* shm_ptr, int rows, int cols){
    for(int i=1; i<rows; i++){
        for(int j=1; j<cols; j++){
            printf("[%d,%d]=[%d]", i,j,*(shm_ptr+i*cols+j));
        }
        printf("\n");
    }
}

int get_shm(int shm_key, int size) {
    errno=0;
    int shm_id = shmget(shm_key, size, IPC_CREAT | IPC_EXCL | 0666 );
    if (shm_id == -1){
        if(errno == EEXIST){ err_exit("ERRORE: memoria condivisa già esistente"); }
        err_exit("ERRORE nella creazione della memoria");
    }
    return shm_id;
}

void *at_shm(int shm_id, int shm_flg) {
    void *shm_ptr = shmat(shm_id, NULL, shm_flg);
    if (shm_ptr == (void *)-1)
        err_exit("ERRORE nell'attach della memoria");
    return shm_ptr;
}

void dt_shm(void *ptr_sh) {
    int shm_dt = shmdt(ptr_sh);
    if (shm_dt == -1)
        err_exit("shmdt failed");
}

void rm_shm(int shm_id) {
    int shm_ctl = shmctl(shm_id, IPC_RMID, NULL);
    if (shm_ctl == -1)
        err_exit("shmctl failed");
}

int get_sem(int sem_key, int sem_num){
    errno=0;
    int sem_id = semget(sem_key,sem_num, IPC_CREAT | 0666 );
    if (sem_id==-1){
        //if(errno==EEXIST) { err_exit("ERRORE: set di semafori già esistente");}
        err_exit("ERRORE nella creazione del set di semafori");
    }
    return sem_id;
}

void ctl_sem(int sem_id, int sem_num){
    errno=0;
    union semun arg;
    unsigned short arr[] = {0};
    arg.array = arr;
    
    int sem_ctl = semctl(sem_id, sem_num, SETALL, arg);
    if (sem_ctl == -1) {
        perror("[SEMCTL]");
        err_exit("ERRORE nell'inizializzazione del semaforo");
    }
}

void usage(){
    printf("uso: F4Server <righe <colonne> <simbolo1> <simbolo2>\n");
}

void err_exit(char *msg){
    printf("%s\n\n\n", msg);
    exit(EXIT_FAILURE);
}



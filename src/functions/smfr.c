#include "../../headers/smfr.h"

int get_sem(int sem_key, int sem_num){
    errno=0;
    int sem_id = semget(sem_key,sem_num, IPC_CREAT | 0777 );
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

void wait_sem(int sem_id, int sem_num) {
    struct sembuf sem_op = (struct sembuf){.sem_num=sem_num, .sem_op=-1, .sem_flg=0};
    errno = 0;
    int semop_ret;
    do {
        if(errno==EINTR) { printf("[WARNING]: system call interrotta, riprovo\n"); }
        semop_ret = semop(sem_id, &sem_op, 1);
    } while (semop_ret==-1 && errno==EINTR);
}

void signal_sem(int sem_id, int sem_num){
    struct sembuf sem_op = (struct sembuf){sem_num,1,0};
    int semop_ret;
    errno = 0;
    do {
        if(errno==EINTR) { printf("[WARNING]: system call interrotta, riprovo\n"); }
        semop_ret = semop(sem_id, &sem_op, 1);
    } while (semop_ret==-1 && errno==EINTR);
}

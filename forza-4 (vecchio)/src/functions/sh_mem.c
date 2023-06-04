#include "../../headers/sh_mem.h"

int create_shm(int shm_key, int size) {
    errno=0;
    int shm_id = shmget(shm_key, size, IPC_CREAT | IPC_EXCL | 0666 );
    if (shm_id == -1){
        if(errno == EEXIST){ err_exit("[ERROR]: memoria condivisa già esistente.\n"); }
        err_exit("[ERROR]: nella creazione della memoria.\n");
    }
    return shm_id;
}

int get_shm(int shm_key, int size) {
    errno=0;
    int shm_id = shmget(shm_key, size, 0666 );
    if (shm_id == -1){
        err_exit("[ERROR]: nella creazione della memoria.\n");
    }
    return shm_id;
}

void *at_shm(int shm_id) {
    void *shm_ptr = shmat(shm_id, NULL, 0);
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
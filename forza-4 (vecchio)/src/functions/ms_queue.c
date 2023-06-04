#include "../../headers/ms_queue.h"

int create_msq(int msq_key){
    errno = 0;
    int msq_id = msgget(msq_key, IPC_CREAT | IPC_EXCL | 0666);
    if(msq_id==-1){
        if(errno == EEXIST){ err_exit("[ERROR]: coda di messaggi già esistente.\n"); }
        err_exit("[ERROR] msgget");
    }
    return msq_id;
}

int get_msq(int msq_key){
    int msq_id = msgget(msq_key, IPC_CREAT | 0666);
    if(msq_id==-1){
        err_exit("[ERROR] msgget");
    }
    return msq_id;
}


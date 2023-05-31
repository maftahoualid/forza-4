#include "../../headers/ms_queue.h"

int get_msq(int msq_key){
    int msq_id = msgget(msq_key, IPC_CREAT | 0666);
    if(msq_id==-1){
        err_exit("[ERROR] msgget");
    }
    return msq_id;
}

void send_msg(int msq_id, struct matrix_dim* dim){
    if(msgsnd(msq_id,dim,sizeof(int),0)==-1){
        err_exit("[ERROR] msgsnd");
    }
}

void receive_msg(int msq_id, struct matrix_dim* dim){
    if(msgrcv(msq_id,dim,sizeof(int),(long)1.0,0)==-1){
        err_exit("[ERROR] msgsnd");
    }
}

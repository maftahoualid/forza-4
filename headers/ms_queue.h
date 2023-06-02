#ifndef _MS_QUEUE_HH
#define _MS_QUEUE_HH

#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include "err_exit.h"

struct matrix_dim {
        long mtype;
        int rows;
        int cols;
        char sym1;
        char sym2;
        int tocca_a_me;
};

struct player {
        long mtype;
        char sym;
        char name[50];
};

struct turno {
    long mtype;
    int turno;
    int win;
};

int create_msq(int msq_key);
int get_msq(int msq_key);
void send_dim(int msq_id, struct matrix_dim* dim);
void receive_dim(int msq_id, struct matrix_dim* dim);
void send_player(int msq_id, struct player* dim);
void receive_player(int msq_id, struct player* dim);


#endif
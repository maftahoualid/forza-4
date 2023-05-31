#ifndef _MS_QUEUE_HH
#define _MS_QUEUE_HH

#include <stdlib.h>
#include <sys/msg.h>
#include "err_exit.h"

struct matrix_dim {
        long mtype;
        int rows;
        int cols;
        char sym;
};

int get_msq(int msq_key);
void send_msg(int msq_id, struct matrix_dim* dim);
void receive_msg(int msq_id, struct matrix_dim* dim);

#endif
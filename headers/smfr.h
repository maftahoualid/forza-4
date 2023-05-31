#ifndef _SMFR_HH
#define _SMFR_HH

#include <stdio.h>
#include <sys/sem.h>
#include <errno.h>
#include "err_exit.h"

union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
};

int get_sem(int sem_key, int sem_num);
void ctl_sem(int sem_id, int sem_num);
void wait_sem(int sem_id, int sem_num);
void signal_sem(int sem_id, int sem_num);

#endif

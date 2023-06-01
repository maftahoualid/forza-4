#ifndef _SH_MEM_HH
#define _SH_MEM_HH

#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "err_exit.h"

int create_shm(int shm_key, int size);
int get_shm(int shm_key, int size);
void *at_shm(int shm_id);
void dt_shm(void *ptr_sh);
void rm_shm(int shm_id);

#endif
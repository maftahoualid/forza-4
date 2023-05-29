#ifndef _INITGAME_HH
#define _INITGAME_HH

void check_args(int argc, char **argv);
void handle_sigint();
void handle_sigalrm();
void close_server();
void usage();
void init_matrix(void *shm_ptr, int rows, int cols);
void print_matrix(void *shm_ptr, int rows, int cols);
int get_shm(int shm_key, int size);
void *at_shm(int shm_id, int shm_flg);
void dt_shm(void *ptr_sh);
void rm_shm(int shm_id);
int get_sem(int sem_key, int sem_num);
void ctl_sem(int sem_id, int sem_num);
void wait_sem(int sem_id);
void signal_sem(int sem_id);
void forza4(int sem_id, int* shm_ptr, int rows, int cols);

union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
};

#endif
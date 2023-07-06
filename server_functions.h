#ifndef SERVER_FUNCTIONS_HH
#define SERVER_FUNCTIONS_HH

#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

extern int sem_ok;
extern int msq_ok;
extern int shm_get_ok;
extern int shm_at_ok;

extern int shm_key, shm_id;
extern char *shm_ptr;
extern int sem_key, sem_id, sem_num;
extern int msq_key, msq_id;

void close_server();
void handle_sigalrm();
void handle_sigint();
int check_win(char* matrix,int rows,int cols);

#endif





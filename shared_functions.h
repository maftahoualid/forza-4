#ifndef SHARED_FUNCTIONS_HH
#define SHARED_FUNCTIONS_HH

#include <stdio.h>
#include <unistd.h>

void usage();
void err_exit();

struct matrix {
    long mtype;
    int rows;
    int cols;
};

struct player_sym {
    long mtype;
    char sym;
}; 

struct move {
    long mtype;
    char sym;
    int pid;
    int row;
    int col;
};

struct win {
    long mtype;
    int value;
    int pid;
};

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#define BOARD_ROWS 6
#define BOARD_COLS 7

int shmid;
int semid;
int (*board)[BOARD_COLS];

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
} sem_attr;

struct sembuf sem_op;

void attachSharedMemory() {
    shmid = shmget(IPC_PRIVATE, sizeof(int[BOARD_ROWS][BOARD_COLS]), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    board = shmat(shmid, 0, 0);
    if (board == (void *) -1) {
        perror("shmat");
        exit(1);
    }
}

void attachSemaphores() {
    semid = semget(IPC_PRIVATE, 1, 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }
}

void printBoard() {
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            printf("| %d ", board[i][j]);
        }
        printf("|\n");
    }
    printf("-----------------------------\n");
}

void cleanup() {
    if (shmdt(board) == -1) {
        perror("shmdt");
    }
}

void handleSignal(int signal) {
    if (signal == SIGINT) {
        cleanup();
        printf("Game terminated.\n");
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <player_name> <player_symbol>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, handleSignal);

    attachSharedMemory();
    attachSemaphores();

    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    if (semop(semid, &sem_op, 1) == -1) {
        perror("semop");
        exit(1);
    }

    printf("Waiting for the second player...\n");

    sem_op.sem_num = 0;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    if (semop(semid, &sem_op, 1) == -1) {
        perror("semop");
        exit(1);
    }

    while (1) {
        sem_op.sem_num = 0;
        sem_op.sem_op = -1;
        sem_op.sem_flg = 0;
        if (semop(semid, &sem_op, 1) == -1) {
            perror("semop");
            exit(1);
        }

        printf("Current board state:\n");
        printBoard();

        sem_op.sem_num = 0;
        sem_op.sem_op = 1;
        sem_op.sem_flg = 0;
        if (semop(semid, &sem_op, 1) == -1) {
            perror("semop");
            exit(1);
        }

        sleep(1);
    }

    cleanup();

    return 0;
}

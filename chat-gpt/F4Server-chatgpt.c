#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>

#define BOARD_ROWS 6
#define BOARD_COLS 7

int board[BOARD_ROWS][BOARD_COLS];
int shmid;
int semid;

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
} sem_attr;

struct sembuf sem_op;

void createSharedMemory() {
    shmid = shmget(IPC_PRIVATE, sizeof(board), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
}

void createSemaphores() {
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    sem_attr.val = 1;
    if (semctl(semid, 0, SETVAL, sem_attr) == -1) {
        perror("semctl");
        exit(1);
    }
}

void initializeBoard() {
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            board[i][j] = 0;
        }
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
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
    }

    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
    }
}

void handleSignal(int signal) {
    if (signal == SIGINT) {
        cleanup();
        printf("Game terminated.\n");
        exit(0);
    }
}

int checkWin() {
    // Check rows
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j <= BOARD_COLS - 4; j++) {
            if (board[i][j] != 0 && board[i][j] == board[i][j + 1] && board[i][j] == board[i][j + 2] &&
                board[i][j] == board[i][j + 3]) {
                return board[i][j];
            }
        }
    }

    // Check columns
    for (int i = 0; i <= BOARD_ROWS - 4; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (board[i][j] != 0 && board[i][j] == board[i + 1][j] && board[i][j] == board[i + 2][j] &&
                board[i][j] == board[i + 3][j]) {
                return board[i][j];
            }
        }
    }

    // Check diagonals (down-right)
    for (int i = 0; i <= BOARD_ROWS - 4; i++) {
        for (int j = 0; j <= BOARD_COLS - 4; j++) {
            if (board[i][j] != 0 && board[i][j] == board[i + 1][j + 1] && board[i][j] == board[i + 2][j + 2] &&
                board[i][j] == board[i + 3][j + 3]) {
                return board[i][j];
            }
        }
    }

    // Check diagonals (up-right)
    for (int i = 3; i < BOARD_ROWS; i++) {
        for (int j = 0; j <= BOARD_COLS - 4; j++) {
            if (board[i][j] != 0 && board[i][j] == board[i - 1][j + 1] && board[i][j] == board[i - 2][j + 2] &&
                board[i][j] == board[i - 3][j + 3]) {
                return board[i][j];
            }
        }
    }

    return 0;
}

void playGame() {
    int currentPlayer = 1;
    int col;
    int winner = 0;

    while (winner == 0) {
        sem_op.sem_num = 0;
        sem_op.sem_op = -1;
        sem_op.sem_flg = 0;
        if (semop(semid, &sem_op, 1) == -1) {
            perror("semop");
            exit(1);
        }

        printf("Player %d, enter the column number (0-%d): ", currentPlayer, BOARD_COLS - 1);
        scanf("%d", &col);

        if (col < 0 || col >= BOARD_COLS) {
            printf("Invalid column. Try again.\n");
            continue;
        }

        int row;
        for (row = BOARD_ROWS - 1; row >= 0; row--) {
            if (board[row][col] == 0) {
                board[row][col] = currentPlayer;
                break;
            }
        }

        sem_op.sem_num = 0;
        sem_op.sem_op = 1;
        sem_op.sem_flg = 0;
        if (semop(semid, &sem_op, 1) == -1) {
            perror("semop");
            exit(1);
        }

        winner = checkWin();

        if (winner != 0) {
            printf("Player %d wins!\n", currentPlayer);
        } else {
            currentPlayer = (currentPlayer == 1) ? 2 : 1;
        }
    }

    cleanup();
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <rows> <cols> <player1_symbol> <player2_symbol>\n", argv[0]);
        exit(1);
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    if (rows < 5 || cols < 5) {
        printf("Minimum board size is 5x5.\n");
        exit(1);
    }

    signal(SIGINT, handleSignal);

    createSharedMemory();
    createSemaphores();
    initializeBoard();

    int pid1 = fork();

    if (pid1 == -1) {
        perror("fork");
        cleanup();
        exit(1);
    } else if (pid1 == 0) {
        // Child process (F4Client 1)
        char *args[] = {"./F4Client", "Player1", argv[3], NULL};
        execv(args[0], args);
        perror("execv");
        cleanup();
        exit(1);
    }

    int pid2 = fork();

    if (pid2 == -1) {
        perror("fork");
        cleanup();
        exit(1);
    } else if (pid2 == 0) {
        // Child process (F4Client 2)
        char *args[] = {"./F4Client", "Player2", argv[4], NULL};
        execv(args[0], args);
        perror("execv");
        cleanup();
        exit(1);
    }

    playGame();

    return 0;
}

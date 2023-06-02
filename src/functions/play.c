#include "../../headers/play.h"

void forza4(int sem_id, int* shm_ptr, int rows, int cols){
    int (*matrix)[cols] = (void*)shm_ptr;
    int player_num = 1;
    int row=0, col=0;
    int winner=0;

    while (winner==0){
        dec_sem(sem_id, 2);
    }

    do {
        if(matrix[0][cols]!=1 || matrix[0][cols]!=0){
            printf("La colonna è piena!\n");
        }
        printf("Inserisci colonna (da 0 a %i): ",cols);
        scanf("%d",&col);
    } while(col<0 || col>cols || matrix[0][cols]!=1 || matrix[0][cols]!=0);

    for(row=rows-1; row>=0; row++){
        if(matrix[row][col] == 0){
            matrix[row][col] = player_num;
            break;
        }
    }

    winner = check_winner(shm_ptr,rows,cols);
    if(winner!=0){
        printf("Vince giocatore %d", player_num);
    } else { player_num = (player_num)?2:1; }

    close_server();
}

int check_winner(int* shm_ptr, int rows, int cols){
    int i,j;

    // ROW WIN
    for (i=0;i<rows;i++){
        for(j=0; j<cols-4;j++){
            int elem = *(shm_ptr+i*cols+j);
            int row_win = ((elem!=0)&&
                          (elem==*(shm_ptr+i*cols+j+1))&&
                          (elem==*(shm_ptr+i*cols+j)+2)&&
                          (elem==*(shm_ptr+i*cols+j)+3) ? 1 : 0);
            if(row_win==1) return elem;
        }
    }
    // COL WIN
    for (i=0;i<rows-4;i++){
        for(j=0; j<cols;j++){
            int elem = *(shm_ptr+i*cols+j);
            int row_win = ((elem!=0)&&
                          (elem==*(shm_ptr+i*(cols+1)+j))&&
                          (elem==*(shm_ptr+i*(cols+2)+j))&&
                          (elem==*(shm_ptr+i*(cols+3)+j)) ? 1 : 0);
            if(row_win==1) return elem;
        }
    }
    // (DIAGONAL OR) NO WIN
    return 0;
}
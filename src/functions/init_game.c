#include "../../headers/init_game.h"

void check_args(int argc, char **argv){
    if(argc!=5){
        usage();
        err_exit("[ERROR]: Mi hai passato un numero errato di argomenti.\n");
    } 
    else if(atoi(argv[1])<5){
        usage();
        err_exit("[ERROR]: La matrice non può avere meno di 5 righe.\n");
    } 
    else if(atoi(argv[2])<5){
        usage();
        err_exit("[ERROR]: La matrice non può avere meno di 5 colonne.\n");
    } 
    else if(strlen(argv[3]) != sizeof(char) || strlen(argv[4]) != sizeof(char)){
        usage();
        err_exit("[ERROR]: i simboli devono essere lunghi 1 carattere ciascuno.\n");
    }
}

void usage(){
    printf("[NOTE] uso: F4Server <righe <colonne> <simbolo1> <simbolo2>\n");
}

void close_server(){
    printf("[NOTE]: .... closing server .... (da implementare)\n");
    // devo comunicare ai client e ripulire memoria e semafori
    exit(0);
};


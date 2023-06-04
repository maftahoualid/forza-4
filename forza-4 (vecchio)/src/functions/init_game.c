#include "../../headers/init_game.h"

void check_server_args(int argc, char **argv){
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
    } else if( strcmp(argv[3],argv[4])==0 ){
        err_exit("[ERROR]: i simboli devono essere diversi.\n");
    } 
}

void check_client_args(int argc, char **argv){
    if(argc!=2){
        usage();
        err_exit("[ERROR]: Mi hai passato un numero errato di argomenti.\n");
    } else if( strcmp(argv[1],"*")==0 ){
        printf("(opz) <Notifica il server di duplicarsi e lanciare una copia automatica del client>");
    } else if ( strlen(argv[1])>50 ){
        err_exit("[ERROR]: Il nome del giocatore è troppo lungo!");
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


#include "../../headers/hndl_signals.h"

int sigint_count = 0;
void handle_sigint(){
    signal(SIGALRM, handle_sigalrm); //GESTISCO IL TIMER PER IL SECONDO CTRL-C
    if(sigint_count==0){
        printf("[WARNING]: CTRL-C 1: premi un'altra volta per terminare il server\n");
        sigint_count++;
        alarm(10);
    } else {
        printf("[NOTE]: CTRL-C 2: termino il server...\n");
        close_server();
    }
}

void handle_sigalrm(){
    sigint_count=0;
}
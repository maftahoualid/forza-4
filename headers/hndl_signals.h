#ifndef _HNDL_SIGNALS_HH
#define _HNDL_SIGNALS_HH

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "init_game.h"

void handle_sigint();
void handle_sigalrm();

#endif
.SILENT:

all:clean server_exe client_exe

server_obj: funzioni_obj
	# genero i file oggetto di F4Server
	gcc -g3 -c -Wall -Wextra "./src/F4Server.c" -o "./obj/server/F4Server.obj"

client_obj:funzioni_obj
	# genero i file oggetto di F4Client
	gcc -g3 -c -Wall -Wextra "./src/F4Client.c" -o "./obj/client/F4Client.obj"

funzioni_obj:
	# genero i file oggetto delle funzioni
	gcc -g3 -c -Wall -Wextra "./src/functions/err_exit.c" -o "./obj/err_exit.obj"
	gcc -g3 -c -Wall -Wextra "./src/functions/hndl_signals.c" -o "./obj/hndl_signals.obj"
	gcc -g3 -c -Wall -Wextra "./src/functions/init_game.c" -o "./obj/init_game.obj"
	gcc -g3 -c -Wall -Wextra "./src/functions/ms_queue.c" -o "./obj/ms_queue.obj"
	gcc -g3 -c -Wall -Wextra "./src/functions/mtrx.c" -o "./obj/mtrx.obj"
	gcc -g3 -c -Wall -Wextra "./src/functions/sh_mem.c" -o "./obj/sh_mem.obj"
	gcc -g3 -c -Wall -Wextra "./src/functions/smfr.c" -o "./obj/smfr.obj"
	gcc -g3 -c -Wall -Wextra "./src/functions/play.c" -o "./obj/play.obj"

server_exe:clean server_obj
	# genero l'eseguibile server
	gcc -g3 -Wall -Wextra "./obj/server/F4Server.obj" \
	"./obj/err_exit.obj" \
	"./obj/hndl_signals.obj" \
	"./obj/init_game.obj" \
	"./obj/ms_queue.obj" \
	"./obj/mtrx.obj" \
	"./obj/sh_mem.obj" \
	"./obj/smfr.obj" \
	"./obj/play.obj" \
	-o "./exe/F4Server"


client_exe:clean client_obj
	# genero l'eseguibile client
	gcc -g3 -Wall -Wextra "./obj/client/F4Client.obj" \
	"./obj/err_exit.obj" \
	"./obj/hndl_signals.obj" \
	"./obj/init_game.obj" \
	"./obj/ms_queue.obj" \
	"./obj/mtrx.obj" \
	"./obj/sh_mem.obj" \
	"./obj/smfr.obj" \
	"./obj/play.obj" \
	-o "./exe/F4Client"
	echo "Compilazione ok!"

clean:
	ipcrm -a
	echo "Rimossi gli IPC"

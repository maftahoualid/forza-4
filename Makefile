
# lancia dalla cartella forza4

# io ce l'ho nel desktop: 
# cd /home/$USER/desktop/forza-4

.SILENT:

all: clean

	# rimuovo tutti i semafori/code di messaggi/memorie condivise
	ipcrm -a

	# rimuovo tutti gli eseguibili
	rm -f ./out/*
	
	# genero i file oggetto di F4Server
	gcc -c -Wall -Wextra "./src/F4Server.c" -o "./obj/server/F4Server.obj"
	
	# genero i file oggetto di F4Client
	gcc -c -Wall -Wextra "./src/F4Client.c" -o "./obj/client/F4Client.obj"
	
	# genero i file oggetto delle funzioni
	gcc -c -Wall -Wextra "./src/functions/err_exit.c" -o "./obj/err_exit.obj"
	gcc -c -Wall -Wextra "./src/functions/init_game.c" -o "./obj/init_game.obj"
	
	# genero l'eseguibile server
	gcc -Wall -Wextra "./obj/server/F4Server.obj" "./obj/err_exit.obj" "./obj/init_game.obj" -o "./exe/F4Server"

	# genero l'eseguibile client
	gcc -Wall -Wextra "./obj/client/F4Client.obj" "./obj/err_exit.obj" "./obj/init_game.obj" -o "./exe/F4Client"
	
	echo "compilazione ok!"
clean:
	ipcrm -a 
	rm -f /out/*
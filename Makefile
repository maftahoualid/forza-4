.SILENT:

all:clean server_exe client_exe

shared_functions:
	gcc -g3 -c -Wall -Wextra shared_functions.c -o shared_functions.obj

server_obj:
	gcc -g3 -c -Wall -Wextra server.c -o server.obj
server_funtions_obj:
	gcc -g3 -c -Wall -Wextra server_functions.c -o server_functions.obj
server_exe: server_obj server_funtions_obj shared_functions
	gcc -g3 -Wall -Wextra server.obj server_functions.obj shared_functions.obj -o server
	echo "server compilato"

client_obj:
	gcc -g3 -c -Wall -Wextra client.c -o client.obj
client_funtions_obj:
	gcc -g3 -c -Wall -Wextra client_functions.c -o client_functions.obj
client_exe: client_obj client_funtions_obj shared_functions
	gcc -g3 -Wall -Wextra client.obj client_functions.obj shared_functions.obj -o client
	echo "client compilato"

clean:
	ipcrm -a
	echo "IPC rimossi"
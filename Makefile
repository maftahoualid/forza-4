
all: clean
	gcc F4Server.c -o F4Server.exe -Wall
	./F4Server.exe 6 7 x o
clean:
	ipcrm -a
	rm -f *.exe 
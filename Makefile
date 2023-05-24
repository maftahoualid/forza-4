
all: clean
	gcc F4Server.c -o F4Server.out -Wall
	./F4Server.out 6 7 x o
clean:
	ipcrm -a
	rm -f *.out
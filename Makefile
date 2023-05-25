
all: clean
	gcc -Wall -Wextra ./F4Server.c ../headers/*.c -o ./out/F4Server.out
	./out/F4Server.out 6 7 x o
clean:
	ipcrm -a
	rm -f /out/*.out
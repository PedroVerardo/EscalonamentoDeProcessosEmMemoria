all:
	gcc -Wall -o main fits.c interpreter.c manage_memory.c queue.c particion.c
	./main
iter:
	gcc -Wall -o inter interpreter.c

f:
	gcc -Wall -o fit fits.c
	./fit

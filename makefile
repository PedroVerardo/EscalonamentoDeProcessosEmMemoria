all:
	gcc -Wall -o main fits.c interpreter.c manage_memory.c queue.c particion.c -lm
	./main
iter:
	gcc -Wall -o inter interpreter.c 
	./inter

f:
	gcc -Wall -o fit fits.c -lm
	./fit

all:
	gcc -Wall -o main main.c interpreter2.c scheduler.c ordered_list.c queue.c
	gcc -Wall -o new_process new_process.c
	./main
iter:
	gcc -Wall -o inter interpreter2.c

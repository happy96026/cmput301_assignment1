CC=gcc

a1shell: error_handling a1shell.c
	$(CC) -c a1shell.c
	$(CC) -o a1shell a1shell.o error_handling.o

error_handling: error_handling.c
	$(CC) -c error_handling.c

clean:
	rm -rf *.o

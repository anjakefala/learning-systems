CC=gcc

build: copy-file.c
	$(CC) -o copy-file copy-file.c

clean:
	rm -f *.o

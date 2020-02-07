CC=gcc

copy-file: copy-file.c
	$(CC) -o copy-file copy-file.c

clean:
	rm -f *.o

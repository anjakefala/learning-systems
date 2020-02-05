CC=gcc

test: writer reader
	./writer

writer: writer.c
	$(CC) -o writer writer.c -lpthread -lrt
	$(CC) -E -o writer.i writer.c -lpthread -lrt

reader: reader.c
	$(CC) -o reader reader.c -lpthread -lrt

clean:
	rm /dev/shm/ringBuffer
	rm /dev/shm/sem.kotekan

CC=gcc

writer: writer.c
	$(CC) -o writer writer.c -lpthread -lrt
	$(CC) -E -o writer.i writer.c -lpthread -lrt

reader: reader.c
	$(CC) -o reader reader.c -lpthread -lrt

clean:
	../tlpi-book/pshm/pshm_unlink ringBuffer
	../tlpi-book/psem/psem_unlink kotekan

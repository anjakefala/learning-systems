CC=g++

test: writer reader
	./writer

writer: writer.cpp
	$(CC) -o writer writer.cpp -lpthread -lrt
	$(CC) -E -o writer.i writer.cpp -lpthread -lrt

reader: reader.cpp
	$(CC) -o reader reader.cpp -lpthread -lrt

clean:
	rm /dev/shm/ringBuffer
	rm /dev/shm/sem.kotekan

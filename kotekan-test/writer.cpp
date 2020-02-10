#include "rw.h"

#define BUF_SIZE 1024   // Size of transfer buffer

// Rick variables

// number of chunks
#define BUFFER_SIZE 20

// 32MB per chunk
#define N_DATA 1024
#define M_DATA 4096

// number of chunks created in this test
#define TEST_LENGTH 1000

// chunk size in MB
#define CHUNK_SIZE N_DATA * M_DATA * sizeof(double) / 1048576

int main(int argc, char *argv[]) {
    sem_t *sem;
    unsigned int sem_value = 1;
    int flags = O_CREAT;
    int perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    // 1. Create the semaphore that is being used by the writer and reader
    // the writer should have first access
    sem = sem_open(SEM_NAME, flags, perms, sem_value);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    CHECK(sem_wait(sem));

    printf("Wait succeeded!\n");

    int fd;
    size_t len = BUF_SIZE;
    time_t* addr;

    // 2. Create the shared memory segment and attach it to the writer's
    // virtual address space at an address chosen by the system
    fd = shm_open(MEM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    CHECK(fd == -1);

    // Resize object to hold buffer
    CHECK(ftruncate(fd, len));
    printf("Resized to %ld bytes\n", (long) len);

    addr = (time_t*) mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // 'fd' is no longer needed
    CHECK(close(fd));

    // Enter a loop that transfers data to the shared
    // memory segment.

    while(true) {
        for (size_t i = 0;i < len; i = i + sizeof(time_t)) {
            CHECK(sem_post(sem));

            struct timeval timestamp;
            gettimeofday(&timestamp, NULL);

            time_t time_us = timestamp.tv_sec * 1000000 + timestamp.tv_usec;
            printf("%lu\n", time_us);

            CHECK(sem_wait(sem));

            memcpy(addr + i, &time_us, sizeof(time_t));

        }
    }
    printf("Done!\n");
    exit(EXIT_SUCCESS);

}

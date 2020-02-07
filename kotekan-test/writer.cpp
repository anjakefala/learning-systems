#include <sys/types.h>  // Type definitions used by many programs
#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Commonly used library functions + EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>     // System calls
#include <string.h>     // String-handling functions

#include <sys/mman.h>   // mmap
#include <semaphore.h>  // semaphores
#include <fcntl.h>      // file descriptors

#include <sys/time.h>

#include "rw.h"

#define BUF_SIZE 1024   // Size of transfer buffer

#define SEM_NAME "kotekan"
#define MEM_NAME "ringBuffer"

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

#include <sys/types.h>  // Type definitions used by many programs
#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Commonly used library functions + EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>     // System calls
#include <string.h>     // String-handling functions

#include <sys/mman.h>   // mmap
#include <semaphore.h>  // semaphores
#include <fcntl.h>      // file descriptors

#define BUF_SIZE 1024   // Size of transfer buffer

#define SEM_NAME "kotekan"
#define MEM_NAME "ringBuffer"

int main(int argc, char *argv[]) {
    sem_t *sem;
    unsigned int sem_value = 1;
    int flags = O_CREAT;
    int perms = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    int fd;
    size_t len = BUF_SIZE;
    char *addr;

    // 1. Create the semaphore that is being used by the writer and reader
    // the writer should have first access
    sem = sem_open(SEM_NAME, flags, perms, sem_value);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    printf("Wait succeeded!\n");

    // 2. Create the shared memory segment and attach it to the writer's
    // virtual address space at an address chosen by the system
    fd = shm_open(MEM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Resize object to hold buffer
    if (ftruncate(fd, len) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    printf("Resized to %ld bytes\n", (long) len);

    addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // 'fd' is no longer needed
    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    int n = 0;

    // Enter a loop that transfers data to the shared
    // memory segment.

    for (int i = 0; i < 10; i++ ) {

        getchar();
        //  - Transfer character into the shared memory segment
        //memcpy(addr+i, "W", 1);

        //  - Release (increment) the lock
        if (sem_post(sem) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }

        //  - Reserve (decrement) the lock
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        //  - Transfer character into the shared memory segment
        memcpy(addr+i*sizeof(int), &n, sizeof(int));
    }
    printf("Done!\n");
    exit(EXIT_SUCCESS);

}

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>

#define BUF_SIZE 1024

#define SEM_NAME "kotekan"
#define MEM_NAME "ringBuffer"

int main(int argc, char *argv[]) {
    // Transfer blocks of data from shared memory to stdout
    sem_t *sem;

    int fd;
    int *addr;
    struct stat sb;

    // Obtain the details of the semaphore set by the writer program
    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Open existing shared memory object
    fd = shm_open(MEM_NAME, O_RDONLY, 0);

    // Use shared memory object size as length argument for mmap()
    // and as number of bytes to write()
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment for read-only access
    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // fd is no longer needed
    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    for (int i=0;; ++i) {
        // Reserve (decrement) the lock
        // should the reader lock?
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        // Write the block of data in the shared memory segment to standard output
        printf("[%d] %d\n", i, *addr);

        // Release (increment) the writer semaphore
        if (sem_post(sem) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }
    }
}

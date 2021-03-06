#include <sys/stat.h>

#include "rw.h"

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    // Transfer blocks of data from shared memory to stdout
    sem_t *sem;

    int fd;
    time_t *addr;
    struct stat sb;

    // Obtain the details of the semaphore set by the writer program
    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Open existing shared memory object
    fd = CHECK(shm_open(MEM_NAME, O_RDONLY, 0));

    // Use shared memory object size as length argument for mmap()
    // and as number of bytes to write()
    CHECK(fstat(fd, &sb));

    // Attach the shared memory segment for read-only access
    addr = (time_t*) mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // fd is no longer needed
    CHECK(close(fd));

    for (int i=0;; ++i) {
        CHECK(sem_wait(sem));

        printf("[%d]", i);
        for (int j = 0; j < sb.st_size; j = j + sizeof(time_t))
            printf("%lu, ", *(addr + j));
        printf("\n");

        CHECK(sem_post(sem));
    }
}

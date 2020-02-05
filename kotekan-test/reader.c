#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>

#include "rw.h"

#define BUF_SIZE 1024

#define SEM_NAME "kotekan"
#define MEM_NAME "ringBuffer"

int main(int argc, char *argv[]) {
    // Transfer blocks of data from shared memory to stdout
    sem_t *sem;

    int fd;
    void *addr;
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
    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // fd is no longer needed
    CHECK(close(fd));

    for (int i=0;; ++i) {
        volatile record_t *rec = addr;

        char msg[256];

        int gen;
        int n = 0;
        do {
            if (gen == 0 || rec->gen == 0) continue;

            n += 1;
            gen = rec->gen;
            if (gen > 0) {
                memcpy(msg, (char *) rec->msg, 256);
            }
            if (rec->gen == gen) {
                sem_wait();
            }
        } while (rec->gen != gen);
        if (n > 1) {
            printf("[%d] %s (took %d tries)\n", gen, msg, n);
        }
        n = 0;
    }
}

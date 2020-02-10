#include <sys/types.h>  // Type definitions
#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Commonly used library functions + EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>     // System calls
#include <string.h>     // String-handling functions
#include <sys/mman.h>   // mmap
#include <semaphore.h>  // semaphores
#include <fcntl.h>      // file descriptors
#include <sys/time.h>   // Time-related functions

#define SEM_NAME "kotekan"
#define MEM_NAME "ringBuffer"


#define CHECK(EXPR) ({ \
    int __r = EXPR; \
    if (__r == -1) { \
        perror(#EXPR); \
        exit(EXIT_FAILURE); \
    } \
    __r; \
})


typedef struct record_t {
    int gen;
    char msg[];
} record_t;


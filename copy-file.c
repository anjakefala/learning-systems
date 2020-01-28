// Write a C program that makes a copy of a file using:
// * standard I/O
// * system calls
// * with pointless locking
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h> // needed for errno()
#include <fcntl.h>
#include <unistd.h> // needed for close()

extern int errno;
#define BUF_LEN 5
// 1024 is a better buffer length, but we want to practice reading all

int main(int argc, char *argv[]) {

    char *txt;

    if (argc == 2) {
        txt = argv[1];
    }
    else {
        txt = "poem.txt";
    }

    int fd = open(txt, O_RDONLY);
    if (fd == -1) {
        perror("open error");
        return 1;
    }

    char *buf = (char*) malloc(1024);
    ssize_t amt_ret;
    int i = 0;

    while((amt_ret = read(fd, (buf + i * BUF_LEN) , BUF_LEN)) != 0) {
        if (amt_ret == -1) {
            if (errno == EINTR)
                continue;
            perror("read error");
            break;
        }
        i += 1;
    }


    printf("read: %s\n", buf);

    // close the stream
    if (close(fd) == -1) {
        perror("close error");
        return 1;
    }

    free(buf);

    return 0;
}

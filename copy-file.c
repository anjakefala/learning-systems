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
#include <string.h>

extern int errno;
#define BLOCK_LEN 1024

int main(int argc, char *argv[]) {

    char *txt;

    // parse if name of file was passed-in through commandline
    if (argc == 2) {
        txt = argv[1];
    }
    else {
        txt = "poem.txt";
    }

    // open file for reading
    int fd = open(txt, O_RDONLY);
    if (fd == -1) {
        perror("open error");
        return 1;
    }

    // read entire file
    size_t buf_len = 10240;
    size_t used = 0; // pointer that keeps track of the last filled spot of the buffer
    ssize_t amt_ret;

    char *tmp_buf, *buf = (char*) malloc(buf_len);

    while(1) {
        // if the next read might overflow the buffer
        if (used + BLOCK_LEN >= buf_len) {
            // allocate a larger block of memory for the buffer
            buf_len = buf_len + 10240;
            tmp_buf = (char*) realloc(buf, buf_len);

            if(tmp_buf != NULL) {
                buf = tmp_buf;
                continue;
            }
            else {
                // if we failed to allocate more memory
                // clean up after yourself, and exit
                free(buf);
                printf("Error (re)allocating memory\n");
                exit (1);
            }

        }

        else {
            amt_ret = read(fd, buf + used, BLOCK_LEN);
            // hit the EOF
            if (amt_ret == 0)
                break;
            if (amt_ret == -1) {
                if (errno == EINTR)
                    continue;
                perror("read error");
                break;
            }
            used += amt_ret;
        }
    }

    // legal C strings must end with '\0'
    tmp_buf = realloc(buf, used + 1);
    if (tmp_buf == NULL) {
        free(buf);
        printf("Error (re)allocating memory\n");
        exit (1);
    }
    buf = tmp_buf;
    buf[used] = '\0';

    // close the stream
    if (close(fd) == -1) {
        perror("close error");
        return 1;
    }

    ssize_t amt_written;

    fd = creat("copied.txt", 0644);

    if (fd == -1) {
        perror("create error:");
        return 1;
    }

    amt_written = write(fd, buf, used + 1);
    if (amt_written == -1) {
        perror("write error:");
        return 1;
    } else if (amt_written != used + 1) {
        printf("A partial write has occurred!\n");
    }

    // close the stream
    if (close(fd) == -1) {
        perror("close error");
        return 1;
    }

    // free allocated memory
    free(buf);

    return 0;
}

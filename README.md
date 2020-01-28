1. learning system calls.
2. making C katas for them.
3. make a toy shared buffer
  
## Ideas
1. copy a file using system calls
2. copy a file using standard I/O
4. saul: do a simple perf test between different hand implementations of file copy
5. ask saul for help for a kata that needs locks

## Next for copy-file.c
1. Try with a file that is 100x larger
2. It will crash, fix that
3. Then increase BUF_LEN to block size
4. Write write() portion of copy-file

## Tools
- valgrind
- gcc
- gdb
- make

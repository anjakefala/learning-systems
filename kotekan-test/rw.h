
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


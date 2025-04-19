#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

#define dbg(...) fprintf(stderr, __VA_ARGS__)
#define die(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        exit(1);                      \
    } while (0)

#endif

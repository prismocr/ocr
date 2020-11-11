#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

// clang-format off

#define exit_on_error(r) \
    if (r != 0) { \
        fprintf(stderr, "%s\n", get_last_error()); \
        exit(EXIT_FAILURE); \
    }

#define return_on_error(r) if (r != 0) { return r; }

// clang-format on

const char *get_last_error();

void set_last_error(char *error);
int set_last_errorf(const char *fmt, ...);

#endif // ERROR_H

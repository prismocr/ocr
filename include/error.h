#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

// clang-format off
#define try(a) if (a != 0){fprintf(stderr, "%s\n", get_last_error()); \
                            exit(EXIT_FAILURE); }
// clang-format on

const char *get_last_error();

void set_last_error(char *error);
int set_last_errorf(const char *fmt, ...);

#endif // ERROR_H

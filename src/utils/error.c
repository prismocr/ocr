#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char last_error[256] = "";

const char *get_last_error() {
    return last_error;
}

void set_last_error(char *error) {
    strcpy(last_error, error);
}

int set_last_errorf(const char *fmt, ...) {
    int ret;
    va_list args;

    va_start(args, fmt);
    ret = vsprintf(last_error, fmt, args);
    va_end(args);

    return ret;
}

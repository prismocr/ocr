#include "error.h"
#include "stdlib.h"
#include "string.h"

static const char *last_error = "";

const char *get_last_error() {
    return last_error;
}

void set_last_error(const char *error) {
    last_error = error;
}

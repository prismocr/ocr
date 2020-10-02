#ifndef ERROR_H
#define ERROR_H

const char *get_last_error();
void set_last_error(char *error);
int set_last_errorf(const char *fmt, ...);

#endif // ERROR_H

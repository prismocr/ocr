#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>

size_t file_size(const char *path);
char *file_read(const char *path);

#endif // FILE_H

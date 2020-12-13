#include <stdio.h>
#include <stdlib.h>

size_t file_size(const char *path) {
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    size_t size = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);
    fclose(f);
    return size;
}

char *file_read(const char *path) {
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    size_t size = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET); /* same as rewind(f); */

    char *content = malloc(size + 1);

    size_t e = fread(content, 1, size, f);
    if (e)
        printf("%ld\n", e);
    fclose(f);

    content[size] = 0;

    return content;
}

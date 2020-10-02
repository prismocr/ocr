CC := gcc
CFLAGS := -std=c99 -Wextra -Wall -Werror -g -pedantic -Wpointer-arith
LDFLAGS := -Iinclude/

.PHONY: all run format

all: src/*.c
	$(CC) $^ $(CFLAGS) $(LDFLAGS)

run: all
	./a.out lena.bmp

format: src/*.c include/*.h
	clang-format --style=file -i $^


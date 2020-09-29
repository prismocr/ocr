CC := gcc
CFLAGS := -Wall -Werror -pedantic -Wpointer-arith
LDFLAGS := -Iinclude/

all:
	$(CC) src/*.c $(CFLAGS) $(LDFLAGS)

format:
	clang-format --style=file -i src/*.c


CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude/
LDLIBS =

SRC := main.c ${shell find src -name "*.c"}
OBJ := ${SRC:.c=.o}
DEP := ${SRC:.c=.d}

all: format main

main: ${OBJ}

.PHONY: clean format

clean:
	@${RM} main ${OBJ} ${DEP}

format: src/*.c include/*.h
	@clang-format --style=file -i $^

-include ${DEP}
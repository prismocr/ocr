EXEC = ocr
CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -Wpointer-arith -Iinclude/
LDFLAGS = -lm
LDLIBS =

SRC := main.c ${shell find src -name "*.c"}
OBJ := ${SRC:.c=.o}
DEP := ${SRC:.c=.d}

all: format ${EXEC} clean

${EXEC}: ${OBJ}
	@$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean format

clean:
	@${RM} ${OBJ} ${DEP}

mrproper: clean
	@${RM} ${EXEC}

format: src/*.c include/*.h
	@clang-format --style=file -i $^

-include ${DEP}
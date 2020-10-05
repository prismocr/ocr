EXEC = ocr
CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude/
LDFLAGS = -lm
LDLIBS =

SRC := main.c ${shell find src -name "*.c"}
OBJ := ${SRC:.c=.o}
DEP := ${SRC:.c=.d}

all: format ${EXEC}

${EXEC}: ${OBJ}
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean format

clean:
	${RM} ${EXEC} ${OBJ} ${DEP}

format: src/*.c include/*.h
	clang-format --style=file -i $^

-include ${DEP}
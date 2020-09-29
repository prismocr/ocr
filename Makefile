# EXEC = ocr
# CPPFLAGS := -MMD
# CC := gcc
# CFLAGS := -Wall -Wextra -std=c99 
# LDFLAGS := -Iinclude/network/

# BUILD_DIR = build/
# SRC_DIRS := src/

# SRC := $(shell find $(SRC_DIRS) -name *.c)
# OBJ := ${SRC:.c=.o}
# DEP := ${SRC:.c=.d}

# all: format ${EXEC}

# ${EXEC}: ${OBJ}
# 	${CC} ${CFLAGS} -o ${BUILD_DIR}/$@ ${SRC}

# ${OBJ}: 

# .PHONY: clean format

# clean:
# 	${RM} main ${OBJ} ${DEP}

# format:
# 	clang-format -style=file -i main.c src/network/*.c include/network/*.h

# -include ${DEP}
# ========================================================================

CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude/
LDLIBS =

SRC = main.c ${shell find src -name "*.c"}
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

.PHONY: clean format

clean:
	${RM} ${EXEC} ${OBJ} ${DEP}

format:
	clang-format --style=file -i src/*.c include/*.h

-include ${DEP}
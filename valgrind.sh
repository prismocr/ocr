#!/bin/bash

if [ $# -lt 1 ] ; then
    echo "Invalid number of arguments."
    exit 1
fi

# Reads arguments into the table `args`
IFS=' ' read -r -a args <<< $@
unset args[0]

CFLAGS="-std=c99 -Wall -Wextra -Werror -Wpedantic"
if [ $1 = "debug" ] ; then
    CFLAGS+=" -g -O0 -DDEBUG"
elif [ $1 = "release" ] ; then
    CFLAGS+=" -O3 -DNDEBUG"
else
    echo "Invalid compilation configuration."
    echo "Use 'debug' or 'release' instead of '$1'."
    exit 1
fi

make TMPCFLAGS="${CFLAGS}" temp

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         ./build/temp/ocr \
         ${args[@]}


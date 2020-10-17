#
# Makefile of prismocr
#
# To build the project run `make`, it will create an executable called **ocr**
# in the **build/** directory
#

# TODO: add support for .c not at the root of src/

CC := gcc
CPPFLAGS := -MMD -Iinclude/
CFLAGS := -std=c99 -Wall -Wextra -Werror -Wpedantic
LDLIBS := -lm

BUILDDIR := build
EXEC := ocr

OBJS := $(patsubst %.c,%.o,$(notdir $(wildcard src/*.c)))

#
# Debug variables
#
DBGCFLAGS := -g -O0 -DDEBUG

DBGDIR := $(BUILDDIR)/debug
DBGOBJDIR := $(DBGDIR)/obj
DBGOBJS := $(addprefix $(DBGOBJDIR)/,$(OBJS))

#
# Release variables
#
RLSCFLAGS := -O3 -DNDEBUG

RLSDIR := $(BUILDDIR)/release
RLSOBJDIR := $(RLSDIR)/obj
RLSOBJS := $(addprefix $(RLSOBJDIR)/,$(OBJS))


.PHONY: all prep remake debug release clean mrproper format

all: prep debug

#
# Debug rules
#
debug: $(DBGDIR)/$(EXEC)

$(DBGDIR)/$(EXEC): $(DBGOBJS)
	$(CC) $^ -o $@ $(CPPFLAGS) $(CFLAGS) $(LDLIBS) $(DBGCFLAGS)

$(DBGOBJDIR)/%.o: src/%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) $(DBGCFLAGS)

#
# Release rules
#
release: $(RLSDIR)/$(EXEC)

$(RLSDIR)/$(EXEC): $(RLSOBJS)
	$(CC) $^ -o $@ $(CCPFLAGS) $(CFLAGS) $(LDLIBS) $(RLSCFLAGS)

$(RLSOBJDIR)/%.o: src/%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS) $(RLSCFLAGS)

#
# Other rules
#
prep:
	@mkdir -p $(DBGDIR) $(RLSDIR) $(DBGOBJDIR) $(RLSOBJDIR)

remake: clean all

clean:
	$(RM) -r $(OBJDIR)

mrproper:
	$(RM) -r $(BUILDDIR)

format: src/*.c include/*.h
	@clang-format --style=file -i $^

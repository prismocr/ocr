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
DBGCFLAGS := $(CFLAGS) -g -O0 -DDEBUG

DBGDIR := $(BUILDDIR)/debug
DBGOBJDIR := $(DBGDIR)/obj
DBGOBJS := $(addprefix $(DBGOBJDIR)/,$(OBJS))

#
# Release variables
#
RLSCFLAGS := $(CFLAGS) -O3 -DNDEBUG

RLSDIR := $(BUILDDIR)/release
RLSOBJDIR := $(RLSDIR)/obj
RLSOBJS := $(addprefix $(RLSOBJDIR)/,$(OBJS))

#
# Test variables
#
TSTCFLAGS := $(CFLAGS) -g -O0 -DDEBUG

TSTDIR := $(BUILDDIR)/test
TSTOBJDIR := $(TSTDIR)/obj
TSTEXEC := $(addprefix $(TSTDIR)/,$(basename $(notdir $(wildcard test/*.c))))
TSTOBJS := $(addprefix $(TSTOBJDIR)/,$(addsuffix .o,$(notdir $(TSTEXEC))))

.PHONY: all prep remake debug release test clean mrproper format cppcheck

all: prep release

#
# Debug rules
#
debug: $(DBGDIR)/$(EXEC)

$(DBGDIR)/$(EXEC): $(DBGOBJS)
	$(CC) $^ -o $@ $(CPPFLAGS) $(DBGCFLAGS) $(LDLIBS)

$(DBGOBJDIR)/%.o: src/%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(DBGCFLAGS) $(DBGCFLAGS)

#
# Release rules
#
release: $(RLSDIR)/$(EXEC)

$(RLSDIR)/$(EXEC): $(RLSOBJS)
	$(CC) $^ -o $@ $(CCPFLAGS) $(RLSCFLAGS) $(LDLIBS)

$(RLSOBJDIR)/%.o: src/%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(RLSCFLAGS)

#
# Test rules
#
test: prep $(TSTEXEC)
	$(foreach TEST,$(filter $(TSTDIR)/%,$^),./$(TEST) &&) echo "All test passed"

$(TSTDIR)/%: $(TSTOBJDIR)/%.o $(filter-out $(DBGOBJDIR)/main.o,$(DBGOBJS)) Unity/src/unity.c
	$(CC) $^ -o $@ $(CPPFLAGS) $(TSTCFLAGS) $(LDLIBS) -IUnity/src

$(TSTOBJDIR)/%.o: test/%.c $(filter-out $(DBGOBJDIR)/main.o,$(DBGOBJS))
	$(CC) -c $^ -o $@ $(CPPFLAGS) $(TSTCFLAGS) -IUnity/src

#
# Other rules
#
prep:
	@mkdir -p $(DBGOBJDIR) $(RLSOBJDIR) $(TSTOBJDIR)

remake: clean all

clean:
	$(RM) -r $(DBGOBJDIR) $(RLSOBJDIR)

mrproper:
	$(RM) -r $(BUILDDIR)

format: src/*.c include/*.h
	@clang-format --style=file -i $^

cppcheck: src/*.c
	@cppcheck --enable=all -q --std=c99 -Iinclude/ $^


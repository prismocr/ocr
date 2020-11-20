#
# Makefile of prismocr
#
# To build the project run `make`, it will create an executable called **ocr**
# in the **build/** directory
#

# TODO: add support for .c not at the root of src/

CC := gcc
CPPFLAGS := -MMD -Iinclude/
CFLAGS := -std=c99 -Wall -Wextra -Werror -Wpedantic $$(pkg-config --cflags gtk+-3.0) 
LDLIBS := -lm $$(pkg-config --libs gtk+-3.0) -export-dynamic -rdynamic

BUILDDIR := build
EXEC := ocr

SRC_SUBDIRS := $(sort $(patsubst src/%,%,$(dir $(wildcard src/*) $(wildcard src/**/*.c))))
OBJS := $(patsubst src/%.c,%.o,$(wildcard src/*.c) $(wildcard src/**/*.c))

#
# Debug variables
#
DBGCFLAGS := $(CFLAGS) -g -O0 -DDEBUG

DBGDIR := $(BUILDDIR)/debug
DBGOBJDIR := $(DBGDIR)/obj
DBGOBJSUBDIRS := $(addprefix $(DBGOBJDIR)/,$(SRC_SUBDIRS))
DBGOBJS := $(addprefix $(DBGOBJDIR)/,$(OBJS))

#
# Release variables
#
RLSCFLAGS := $(CFLAGS) -O3 -DNDEBUG

RLSDIR := $(BUILDDIR)/release
RLSOBJDIR := $(RLSDIR)/obj
RLSOBJSUBDIRS := $(addprefix $(RLSOBJDIR)/,$(SRC_SUBDIRS))
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
	@if [ ! -d "Unity" ]; then git clone https://github.com/ThrowTheSwitch/Unity.git; fi;
	$(foreach TEST,$(filter $(TSTDIR)/%,$^),./$(TEST) &&) echo "All test passed"

$(TSTDIR)/%: $(TSTOBJDIR)/%.o $(filter-out $(DBGOBJDIR)/main.o,$(DBGOBJS)) Unity/src/unity.c
	$(CC) $^ -o $@ $(CPPFLAGS) $(TSTCFLAGS) $(LDLIBS) -IUnity/src

$(TSTOBJDIR)/%.o: test/%.c $(filter-out $(DBGOBJDIR)/main.o,$(DBGOBJS))
	$(CC) -c $^ -o $@ $(CPPFLAGS) $(TSTCFLAGS) -IUnity/src

#
# Other rules
#
prep:
	@mkdir -p $(DBGOBJDIR) $(RLSOBJDIR) $(TSTOBJDIR) $(DBGOBJSUBDIRS) $(RLSOBJSUBDIRS)

remake: clean all

clean:
	$(RM) -r $(DBGOBJDIR) $(RLSOBJDIR)

mrproper:
	$(RM) -r $(BUILDDIR)

format: src/*.c src/**/*.c include/*.h test/*.c
	@clang-format --style=file -i $^

cppcheck: src/*.c src/**/*.c
	@cppcheck --enable=warning,style,performance,portability,information,\
	missingInclude -q --std=c99 -Iinclude/ $^


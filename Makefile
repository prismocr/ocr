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

SRC_SUBDIRS := $(sort $(patsubst src/%,%,$(dir $(wildcard src/*) $(wildcard src/**/*.c))))
OBJS := $(patsubst src/%.c,%.o,$(wildcard src/*.c) $(wildcard src/**/*.c))

#
# Debug variables
#
DBGCFLAGS := $(CFLAGS) -g -O0 -DDEBUG -fsanitize=address

DBGDIR := $(BUILDDIR)/debug
DBGOBJDIR := $(DBGDIR)/obj
DBGOBJSUBDIRS := $(addprefix $(DBGOBJDIR)/,$(SRC_SUBDIRS))
DBGOBJS := $(addprefix $(DBGOBJDIR)/,$(OBJS))

#
# Release variables
#
RLSCFLAGS := $(CFLAGS) -O3 -DNDEBUG -static

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

#
# Temp variables
#
TMPDIR := $(BUILDDIR)/temp
TMPOBJDIR := $(TMPDIR)/obj
TMPOBJSUBDIRS := $(addprefix $(TMPOBJDIR)/,$(SRC_SUBDIRS))
TMPOBJS := $(addprefix $(TMPOBJDIR)/,$(OBJS))

.PHONY: all prep remake debug release test clean mrproper format cppcheck temp prep-temp

all: release

#
# Debug rules
#
debug: prep $(DBGDIR)/$(EXEC)

$(DBGDIR)/$(EXEC): $(DBGOBJS)
	$(CC) $^ -o $@ $(CPPFLAGS) $(DBGCFLAGS) $(LDLIBS)

$(DBGOBJDIR)/%.o: src/%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(DBGCFLAGS)

#
# Release rules
#
release: prep $(RLSDIR)/$(EXEC)

$(RLSDIR)/$(EXEC): $(RLSOBJS)
	$(CC) $^ -o $@ $(CPPFLAGS) $(RLSCFLAGS) $(LDLIBS)

$(RLSOBJDIR)/%.o: src/%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(RLSCFLAGS)

#
# Test rules
#
test: prep $(TSTEXEC)
	@if [ ! -d "Unity" ]; then git clone https://github.com/ThrowTheSwitch/Unity.git; fi;
	$(foreach TEST,$(filter $(TSTDIR)/%,$^),./$(TEST) &&) echo "All test passed"

$(TSTDIR)/%: $(TSTOBJDIR)/%.o $(filter-out $(RLSOBJDIR)/main.o,$(RLSOBJS)) Unity/src/unity.c
	$(CC) $^ -o $@ $(CPPFLAGS) $(TSTCFLAGS) $(LDLIBS) -IUnity/src

$(TSTOBJDIR)/%.o: test/%.c $(filter-out $(RLSOBJDIR)/main.o,$(RLSOBJS))
	$(CC) -c $^ -o $@ $(CPPFLAGS) $(TSTCFLAGS) -IUnity/src

#
# Other rules
#
prep:
	@mkdir -p $(TSTOBJDIR) $(DBGOBJSUBDIRS) $(RLSOBJSUBDIRS)

remake: clean all

clean:
	$(RM) -r $(DBGOBJDIR) $(RLSOBJDIR)

mrproper:
	$(RM) -r $(BUILDDIR)

format: src/*.c src/**/*.c include/*.h include/**/*.h test/*.c
	@clang-format --style=file -i $^

cppcheck: src/*.c src/**/*.c
	@cppcheck --enable=warning,style,performance,portability,information,\
	missingInclude -q --std=c99 -Iinclude/ $^

temp: prep-temp $(TMPDIR)/$(EXEC)

prep-temp:
	@if [ -d $(TMPDIR) ] ; then rm -r $(TMPDIR) ; fi
	@mkdir -p $(TMPOBJSUBDIRS)

$(TMPDIR)/$(EXEC): $(TMPOBJS)
	$(CC) $^ -o $@ $(CPPFLAGS) $(TMPCFLAGS) $(LDLIBS)

$(TMPOBJDIR)/%.o: src/%.c
	$(CC) -c $< -o $@ $(CPPFLAGS) $(TMPCFLAGS)


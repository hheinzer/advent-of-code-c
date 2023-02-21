#
# See LICENSE file for copyright and license details.
#

# configuration {on, off}
assert   = on
debug    = on
analyzer = off
sanitize = on
profile  = off

# compiler
CC = gcc
AR = gcc-ar rcs

#
# compilation recipes
#

# targets
.PHONY: default lib run clean check memcheck format

# default target
default: run

# default flags
CFLAGS = -std=c11 -pedantic -g -Wall -Wextra

# additional warnings
CFLAGS += -Wshadow -Wfloat-equal -Wundef -Wunreachable-code -Wswitch-default \
		  -Wswitch-enum -Wpointer-arith -Wno-missing-braces -Wwrite-strings

# included directories
INCS = -Iaoc

# assert flags
ifneq ($(assert), on)
CFLAGS += -DNDEBUG
CFLAGS += -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter
endif

# optimization flags
ifeq ($(debug), on)
CFLAGS += -Og -fno-omit-frame-pointer
ifeq ($(analyzer), on)
CFLAGS += -fanalyzer
endif
else
CFLAGS += -Winline
CFLAGS += -march=native -mtune=native
CFLAGS += -O3 -ffast-math -funroll-loops
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -flto=auto
endif

# sanitation flags
ifeq ($(sanitize), on)
CFLAGS += -fsanitize=undefined -fsanitize=address
endif

# profiler flags
ifeq ($(profile), on)
CFLAGS += -pg -fno-lto
endif

# linking
LDFLAGS = -Wl,--gc-sections
LDLIBS  = -lm

# objects
SRC = $(shell find aoc -type f -name '*.c')
OBJ = $(SRC:%.c=%.o)

# library
LIB = aoc/libaoc.a

# binaries
RUN = $(shell find 20* -type f -name '*.c')
BIN = $(RUN:%.c=%)

# dependencies
CFLAGS += -MMD -MP
DEP = $(OBJ:.o=.d) $(BIN:=.d)
-include $(DEP)

# build objects
$(OBJ): %.o: %.c Makefile
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

# build library
lib: $(LIB)

$(LIB): $(OBJ)
	$(AR) $@ $^

# build binaries
run: $(BIN)

$(BIN): %: %.c $(LIB) Makefile
	-$(CC) $(CFLAGS) $(INCS) $(LDFLAGS) $< $(LIB) $(LDLIBS) -o $@

# run all
run: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done

# auxiliary functions
clean:
	rm -rf $(OBJ) $(BIN) $(DEP) $(LIB) gmon.out perf.data*

check:
	-cppcheck --enable=all --inconclusive --suppress=missingIncludeSystem \
		--suppress=unreadVariable --suppress=unusedFunction \
		-Iaoc aoc 20*

memcheck:
	-valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--suppressions=.memcheck.supp $(BIN)

format:
	-clang-format -i $(shell find . -type f -name '*.c' -o -name '*.h')

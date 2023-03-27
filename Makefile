# See LICENSE file for copyright and license details.

# configuration {on, off}
debug    = on
profile  = off

# compiler
CC = gcc
AR = gcc-ar rcs

# default flags
CFLAGS  = -std=c11 -g3 -Wall -Wextra -Wpedantic
CFLAGS += -Wshadow -Wfloat-equal -Wundef -Wunreachable-code -Wswitch-default
CFLAGS += -Wswitch-enum -Wpointer-arith -Wwrite-strings -Wstrict-prototypes

# included directories
INCS = -Iaoc

# optimization flags
ifeq ($(debug), on)
CFLAGS += -Og
CFLAGS += -fsanitize=undefined,address
CFLAGS += -fanalyzer
else
CFLAGS += -O2 -march=native -flto=auto -DNDEBUG #-Wno-return-type
endif

# profiler flags
ifeq ($(profile), on)
CFLAGS += -pg -fno-lto
endif

# linking
LDLIBS  = -lm

# objects
SRC = $(shell find aoc -type f -name '*.c')
OBJ = $(SRC:%.c=%.o)

# library
LIB = aoc/libaoc.a

# binaries
RUN = $(shell find 20* -type f -name '*.c')
BIN = $(RUN:%.c=%)

# default target
default: $(BIN)

# dependencies
CFLAGS += -MMD -MP
DEP = $(OBJ:.o=.d) $(BIN:=.d)
-include $(DEP)

# build objects
$(OBJ): %.o: %.c Makefile
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

# build library
$(LIB): $(OBJ)
	$(AR) $@ $^

# build binaries
$(BIN): %: %.c $(LIB) Makefile
	-$(CC) $(CFLAGS) $(INCS) $< $(LIB) $(LDLIBS) -o $@

# functions
clean:
	rm -rf $(BIN) $(LIB) $(OBJ) $(DEP) gmon.out perf.data*

check:
	-cppcheck --enable=all --inconclusive --suppress=missingIncludeSystem \
		--suppress=unusedFunction --project=compile_commands.json

memcheck:
	-valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--suppressions=.memcheck.supp $(BIN)

run: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done

test: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done | grep -v wtime | diff --color=auto solutions.txt - \
	&& echo "*** All tests passed. ***"

solutions: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done | grep -v wtime > solutions.txt

format:
	-clang-format -i $(shell find . -type f -name '*.c' -o -name '*.h')

.PHONY: default clean check memcheck run test solutions format

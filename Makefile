# See LICENSE file for copyright and license details.

# configuration {on, off}
debug = on

# compiler
CC = gcc

# default flags
CFLAGS = -std=c11 -g3 -Wall -Wextra -Wpedantic

# included directories
INCS = -Iaoc

# optimization flags
ifeq ($(debug), on)
CFLAGS += -Og
CFLAGS += -fsanitize=undefined,address -fsanitize-undefined-trap-on-error
CFLAGS += -fanalyzer -Wno-analyzer-imprecise-fp-arithmetic
else
CFLAGS += -march=native -Ofast -flto=auto
CFLAGS += -DNDEBUG
endif

# linking
LDLIBS  = -lm

# objects
SRC = $(shell find aoc -type f -name '*.c')
OBJ = $(SRC:%.c=%.o)

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

# build binaries
$(BIN): %: %.c $(OBJ) Makefile
	-$(CC) $(CFLAGS) $(INCS) $< $(OBJ) $(LDLIBS) -o $@

# functions
clean:
	rm -rf $(BIN) $(OBJ) $(DEP) gmon.out perf.data*

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

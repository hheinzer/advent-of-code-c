# compiler and default flags
CC = clang
CFLAGS = -std=c23 -g3 -Wall -Wextra -Wpedantic -Wshadow -Wno-unused-function

# debug flags
CFLAGS += -fsanitize=undefined,address

# release flags
#CFLAGS += -O3 -march=native -flto=auto

# libraries
LDLIBS = -lm

# sources, objects, and programs
RUN = $(shell find 20* -type f -name '*.c')
BIN = $(patsubst %.c, %, $(RUN))

# make functions
.PHONY: all clean check run test solutions
all: $(BIN)

clean:
	@rm -rf $(BIN)

check:
	@cppcheck --quiet --project=compile_commands.json --inline-suppr \
		--enable=all --inconclusive --check-level=exhaustive \
		--suppress=checkersReport --suppress=missingIncludeSystem \
		--suppress=unusedFunction --suppress=shadowFunction \
		--suppress=constVariablePointer

run: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done

test: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done | diff --color=auto -U 2 solutions.txt - \
	&& echo "*** All tests passed. ***"

solutions: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done > solutions.txt

# build rules
.SUFFIXES:
%: %.c Makefile
	-@$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

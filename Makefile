# compiler and default flags
CC = clang
CFLAGS = -std=c23 -g -Wall -Wextra -Wpedantic -Wshadow -Wno-unused-function

# debug flags
CFLAGS += -Og -fno-omit-frame-pointer -fsanitize=undefined
#CFLAGS += -fsanitize=address
#CFLAGS += -fsanitize=memory -fPIE -pie

# release flags
#CFLAGS += -march=native -Ofast -flto=auto -DNDEBUG

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
	@cppcheck --quiet --project=compile_commands.json \
		--enable=all --inconclusive --check-level=exhaustive \
		--suppress=missingIncludeSystem --suppress=checkersReport \
		--suppress=unusedFunction

run: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done

test: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done | grep -v wtime | diff --color=auto -U 2 solutions.txt - \
	&& echo "*** All tests passed. ***"

solutions: $(BIN)
	@for prog in $(sort $(BIN)); do \
		echo "--- $$prog ---" && \
		./$$prog; \
	done | grep -v wtime > solutions.txt

# build rules
.SUFFIXES:
%: %.c Makefile
	-@$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

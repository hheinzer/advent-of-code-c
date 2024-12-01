# See LICENSE file for copyright and license details.
CC = clang
CFLAGS = -std=c23 -g -Isrc -Wall -Wextra -Wpedantic -Wshadow

# debug flags
CFLAGS += -Og -fno-omit-frame-pointer

# release flags
#CFLAGS += -march=native -Ofast -flto=auto -DNDEBUG

# libraries
LDLIBS = -lm

# sources, objects, and programs
SRC = $(shell find src -type f -name '*.c')
RUN = $(shell find 20* -type f -name '*.c')
OBJ = $(patsubst %.c, %.o, $(SRC))
BIN = $(patsubst %.c, %, $(RUN))

# dependencies
CFLAGS += -MMD -MP
DEP = $(OBJ:.o=.d) $(BIN:=.d)
-include $(DEP)

# make functions
.PHONY: all clean check format tidy run test solutions
all: $(BIN)

clean:
	@rm -rf $(BIN) $(OBJ) $(DEP)

check:
	@cppcheck --quiet --project=compile_commands.json \
		--enable=all --inconclusive --check-level=exhaustive \
		--suppress=checkersReport --suppress=missingIncludeSystem --suppress=unusedFunction

format:
	@clang-format -i $(shell find . -type f -name '*.[ch]')

tidy:
	@clang-tidy --quiet $(shell find . -type f -name '*.[ch]')

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

# build rules
.SUFFIXES:
%.o: %.c Makefile
	@$(CC) $(CFLAGS) -c $< -o $@

%: %.c $(OBJ)
	-@$(CC) $(CFLAGS) $< $(OBJ) $(LDLIBS) -o $@

# compiler and default flags
CC = clang
CFLAGS = -std=c23 -g -Wall -Wextra -Wpedantic -Wshadow -Wno-unused-function

# debug flags
CFLAGS += -Og -fno-omit-frame-pointer -fsanitize=undefined
CFLAGS += -fsanitize=address
#CFLAGS += -fsanitize=memory -fPIE -pie

# release flags
#CFLAGS += -march=native -Ofast -flto=auto -DNDEBUG

# libraries
LDLIBS = -lm

# sources, objects, and programs
RUN = $(shell find 20* -type f -name '*.c')
BIN = $(patsubst %.c, %, $(RUN))

# make functions
.PHONY: all clean run test solutions
all: $(BIN)

clean:
	@rm -rf $(BIN)

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
%: %.c $(OBJ)
	-@$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

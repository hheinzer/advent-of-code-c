CC=gcc
CFLAGS+=-std=c11 -pedantic -g -Wall -Wextra -Og
CFLAGS+=-Wshadow -Wfloat-equal -Wundef -Wunreachable-code -Wswitch-default \
        -Wswitch-enum -Wpointer-arith -Wno-missing-braces -Wwrite-strings

#CFLAGS+=-fsanitize=undefined -fsanitize=address

#CFLAGS+=-Winline \
#		-march=native -mtune=native \
#		-O3 -ffast-math -funroll-loops \
#		-fdata-sections -ffunction-sections \
#		-flto=auto

SRC=$(shell find . -type f -name '*.c')
BIN=$(SRC:%.c=%)

.PHONY: all clean format

all:
	@for src in *.c; do \
		prog=$$(basename $$src .c) && \
		echo "--- $$prog ---" && \
		$(MAKE) -s $$prog && \
		./$$prog; \
	done

DEP=../aoc/aoc.o ../aoc/list.o ../aoc/stack.o ../aoc/queue.o ../aoc/dict.o

$(BIN): $(DEP)

clean:
	-rm -rf $(BIN) $(DEP)

format:
	-clang-format -i $(shell find . -type f -name '*.c' -o -name '*.h')

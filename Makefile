CC=gcc
CFLAGS+=-std=c11 -pedantic -g -Wall -Wextra -Og
CFLAGS+=-Wshadow -Wfloat-equal -Wundef -Wunreachable-code -Wswitch-default \
        -Wswitch-enum -Wpointer-arith -Wno-missing-braces -Wwrite-strings

CFLAGS+=-fsanitize=undefined -fsanitize=address

#CFLAGS+=-Winline \
#		-march=native -mtune=native \
#		-O3 -ffast-math -funroll-loops \
#		-fdata-sections -ffunction-sections \
#		-flto=auto

.PHONY: default all clean format

default: all

SRC=$(shell find . -type f -name '*.c')
BIN=$(SRC:%.c=%)

$(BIN): ../aoc/aoc.o ../aoc/list.o ../aoc/htable.o

all:
	@for src in *c; do \
		[ $$src = "aoc.c" ] || (\
			prog=$$(basename $$src .c) && \
			echo "--- $$prog ---" && \
			$(MAKE) -s $$prog && \
			./$$prog \
		); \
	done

clean:
	-rm -rf $(BIN)
	-find . -type f -name '*.o'

format:
	-clang-format -i $(shell find . -type f -name '*.c' -o -name '*.h')

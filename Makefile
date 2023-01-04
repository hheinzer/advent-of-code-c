CC=gcc
CFLAGS+=-std=c11 -pedantic -g -Wall -Wextra
CFLAGS+=-Wshadow -Wfloat-equal -Wundef -Wunreachable-code -Wswitch-default \
        -Wswitch-enum -Wpointer-arith -Wno-missing-braces -Wwrite-strings
CFLAGS+=-fsanitize=undefined -fsanitize=address

.PHONY: all clean

all:
	@for src in *c; do \
		[ $$src = "aoc.c" ] || (\
			prog=$$(basename $$src .c) && \
			echo "--- $$prog ---" && \
			make -s $$prog && \
			./$$prog \
		); \
	done

clean:
	find . -maxdepth 2 -type f -perm -u+x -delete

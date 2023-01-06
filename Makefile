CC=gcc
CFLAGS+=-std=c11 -pedantic -g -Wall -Wextra
CFLAGS+=-Wshadow -Wfloat-equal -Wundef -Wunreachable-code -Wswitch-default \
        -Wswitch-enum -Wpointer-arith -Wno-missing-braces -Wwrite-strings

CFLAGS+=-fsanitize=undefined -fsanitize=address

#CFLAGS+=-Winline \
#		-march=native -mtune=native \
#		-O3 -ffast-math -funroll-loops \
#		-fdata-sections -ffunction-sections \
#		-flto=auto

.PHONY: all clean format

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

format:
	-clang-format -i $(shell find . -type f -name '*.c' -o -name '*.h')

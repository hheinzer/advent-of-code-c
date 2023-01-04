CC=gcc
CFLAGS+=-std=c11 -pedantic -g -Wall -Wextra
CFLAGS+=-Wshadow -Wfloat-equal -Wundef -Wunreachable-code -Wswitch-default \
        -Wswitch-enum -Wpointer-arith -Wno-missing-braces -Wwrite-strings
CFLAGS+=-fsanitize=undefined -fsanitize=address

.PHONY: clean

clean:
	find . -maxdepth 2 -type f -perm -u+x -delete

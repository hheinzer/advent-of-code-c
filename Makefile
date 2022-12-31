CC=gcc
CFLAGS+=-std=gnu11 -pedantic -g -Wall -Wextra
CFLAGS+=-Wshadow -Wfloat-equal -Wundef -Wunreachable-code -Wswitch-default \
        -Wswitch-enum -Wpointer-arith -Wno-missing-braces -Wwrite-strings
CFLAGS+=-fsanitize=undefined -fsanitize=address

.PHONY: clean

clean:
	find 20*/. -type f -perm -u+x -delete

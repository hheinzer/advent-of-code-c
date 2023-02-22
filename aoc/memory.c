#include "memory.h"

void *malloc_assert(size_t size);

void *calloc_assert(size_t nmemb, size_t size);

void *realloc_assert(void *ptr, size_t size);

void *memdup(const void *src, size_t size)
{
    return memcpy(malloc(size), src, size);
}

char *strdup(const char *src)
{
    return memdup(src, strlen(src) + 1);
}

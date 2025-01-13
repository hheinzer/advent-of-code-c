#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void *malloc_assert(size_t size) {
    void *ptr = malloc(size);
    assert(ptr && "Could not malloc memory.");
    return ptr;
}
#define malloc malloc_assert

void *calloc_assert(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    assert(ptr && "Could not calloc memory.");
    return ptr;
}
#define calloc calloc_assert

void *realloc_assert(void *ptr, size_t size) {
    ptr = realloc(ptr, size);
    assert(ptr && "Could not realloc memory.");
    return ptr;
}
#define realloc realloc_assert

void *memdup(const void *src, size_t size) {
    void *dest = malloc(size);
    void *ret = memcpy(dest, src, size);
    assert(ret);
    return dest;
}

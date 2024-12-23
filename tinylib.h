#pragma once
#include "syscalls.h"

#define nullptr ((void*)0)
#define STDOUT 1
#define STDIN 0
#define STDERR 2


int t_strlen(const char * str) {
    if(str == nullptr) return -1;
    int len = 0;
    while(*str++) {
        len++;
    }
    return len;
}

int t_puts(const char * str) {
    if(str == nullptr) return 0;
    return s_write(STDOUT, str, t_strlen(str));
}

uint64_t t_fsize(int fd) {
    uint64_t fstat[18];
    s_fstat(fd, fstat);
    return fstat[6];
}

typedef struct {
    void* ptr;
    uint64_t size;
    uint64_t used;
} Arena;

//size 0 for default
Arena arena_create(uint64_t size) {
    if(size == 0) size = 20000;
    
    Arena arena;
    arena.ptr = s_mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    arena.size = size;
    arena.used = 0;

    return arena;
}

void* arena_allocate(Arena* arena, uint64_t size) {
    if(arena->size - arena->used < size) return nullptr;
    if(size == 0) return nullptr;
    void* ptr = arena->ptr + size;
    arena->used += size;
    return ptr;
}

void arena_delete(Arena* arena) {
    if(!arena->ptr) return;
    s_unmap(arena->ptr, arena->size);
    arena->ptr = nullptr;
    arena->size = arena->used = 0;
}

#pragma once
#include <elf.h>
#include "syscalls.h"


void* find_unused_memory(const Elf64_Phdr* phdr, int phnum) {
    uint64_t candidate = 0;
    for(int i = 0; i < phnum; i++) {
        if(phdr[i].p_type != PT_LOAD) continue;
        
        uint64_t new_cand = phdr[i].p_vaddr + phdr[i].p_memsz;

        if(new_cand > candidate) {
            candidate = new_cand;
        }
    }

    return (void*)candidate;
}


void* mmap_above(void* addr, uint64_t size) {
    void* ptr = s_mmap(addr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if(ptr == (void*)0) return ptr;

    while((uint64_t)ptr < (uint64_t)addr) {
        // page aligned...
        addr -= ((uint64_t)addr % 0x1000);
        addr += 0x1000;
        ptr = s_mmap(addr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    }
    return ptr;
}


void copy_chunk(char* restrict dest, const char* restrict src, uint64_t size) {
    //I love premature optimization <3
    while(size >= 32) {
        uint64_t* restrict d = (uint64_t* restrict)dest;
        const uint64_t* restrict s = (const uint64_t* restrict)src;

        d[0] = s[0];
        d[1] = s[1];
        d[2] = s[2];
        d[3] = s[3];

        size -= 32;
        dest += 32;
        src += 32;
    }
    while(size--) {
        *dest++ = *src++;
    }
}

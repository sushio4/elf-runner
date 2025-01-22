#pragma once
#include "syscalls.h"
#include "utils_loaded.h"

#define prnt(STR) {char s[] = (STR); puts(s);}

static int strlen(const char * str) {
    if(str == nullptr) return 0;
    int len = 0;
    while(*str++) {
        len++;
    }
    return len;
}

static int puts(const char * str) {
    if(str == nullptr) return 0;
    return s_write(STDOUT, str, strlen(str));
}

// str HAS TO HAVE AT LEAST 17 BYTES ALLOCATED!
static void t_int_to_str(uint64_t val, char* str) {
    str[16] = '\0';
    char lookup[] = "0123456789ABCDEF";
    for(char* p = str+15; p >= str; val>>=4, p--) {
        *p = lookup[val & 0xF];
    }
}

static void hexprint(uint64_t val) {
    char num[17];
    t_int_to_str(val, num);
    s_write(STDOUT, num, 16);
}

static void* mmap_above(void* addr, uint64_t size, uint64_t prot, uint64_t flags, int fd, uint64_t pgoff) {
    if(prot == 0) prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    if(flags == 0) flags = MAP_ANONYMOUS | MAP_PRIVATE;
    
    void* ptr = s_mmap(addr, size, prot, flags, fd, pgoff);
    while((uint64_t)ptr < (uint64_t)addr) {
        addr -= (uint64_t)addr % 0x1000;
        
        if(ptr != (void*)0)
            s_unmap(ptr, size);

        addr += 0x1000;
        ptr = s_mmap(addr, size, prot, flags, fd, pgoff);
    }
    return ptr;
}

static uint64_t fsize(int fd) {
    uint64_t fstat[18];
    s_fstat(fd, fstat);
    return fstat[6];
}

static void copy_chunk(char* restrict dest, const char* restrict src, uint64_t size) {
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

static uint64_t load_program(Elf64_Phdr* phdr, int phnum, char* file) {
    #ifdef DEBUG
    prnt("Loading program...\n");
    #endif

    for(int i = 0; i < phnum; i++) {
        if(phdr[i].p_type != PT_LOAD) continue;
        
        if(phdr[i].p_memsz == 0) continue;

        //addr alignment
        uint64_t addr = phdr[i].p_vaddr;
        addr = addr - addr % phdr[i].p_align;

        uint64_t vsize = phdr[i].p_vaddr - addr + phdr[i].p_memsz;

        uint64_t len = 0;
        while(len < vsize) {
            len += phdr[i].p_align;
        }
        
        char endl[] = "\n";

        int ret = s_unmap((void*)addr, len); 
        if(ret != 0) {
            char s[] = "Couldn't delete that memory :/\nlen: ";
            puts(s);
            hexprint(len);
            char s2[] = "\naddr: ";
            puts(s2);
            hexprint(addr);
            puts(endl);
            s_exit(ret);
        }
        #ifdef DEBUG
        char s[] = "Memory unmapped at: ";
        puts(s);
        hexprint(addr);
        puts(endl);
        #endif

        void* ptr = s_mmap((void*)addr, len, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0);
        if((uint64_t)ptr != addr) {
            char s[] = "Could not map memory there ";
            puts(s);
            #ifdef DEBUG
            hexprint(addr);
            char s2[] = "\nMapped instead at: ";
            puts(s2);
            hexprint((uint64_t)ptr);
            #endif 
            puts(endl);
            s_exit(1);
        }

        copy_chunk((char*)phdr[i].p_vaddr, file + phdr[i].p_offset, phdr[i].p_filesz);

        // translate from elf flags to mmap flags
        uint64_t flags = 0;
        if(phdr[i].p_flags & PF_R) flags |= PROT_READ;
        if(phdr[i].p_flags & PF_W) flags |= PROT_WRITE;
        if(phdr[i].p_flags & PF_X) flags |= PROT_EXEC;

        ret = s_mprotect((void*)addr, len, flags);
        if(ret != 0) {
            char s[] = "Could not set protection!\n";
            puts(s);
            s_exit(1);
        }

        #ifdef DEBUG
        char s2[] = "Loaded successfully!\nFrom: ";
        puts(s2);
        hexprint((uint64_t)ptr);
        char s3[] = "\nTo: ";
        puts(s3);
        hexprint((uint64_t)ptr + len);
        puts(endl);
        #endif
    }

    return 0;
}

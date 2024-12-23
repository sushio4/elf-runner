#pragma once
#include <stdint.h>

#pragma GCC push_options
#pragma GCC optimize("-O1")

static uint64_t s_exit(int code) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $60, %%al\n"
        "syscall\n"::
    );
}


static uint64_t s_read(uint64_t fd, const void* dest, uint64_t n) {
    asm(
        "xor    %%rax, %%rax\n"
        "syscall\n"::
    );
}

static uint64_t s_write(uint64_t fd, const void* src, uint64_t n) {
    asm(
        "xor    %%rax, %%rax\n"
        "inc    %%eax\n"
        "syscall\n"::
    );
}

#define DT_REG 8

static uint64_t s_getdents64(uint64_t fd, const void* dest, uint64_t count) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $217, %%al\n"
        "syscall\n"::
    );
}

static uint64_t s_pwrite64(uint64_t fd, const void* src, uint64_t n, uint64_t off) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $18, %%al\n"
        "movq   %%rcx, %%r10\n"
        "syscall\n"::
    );
}

static uint64_t s_pread64(uint64_t fd, const void* dest, uint64_t n, uint64_t off) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $17, %%al\n"
        "movq   %%rcx, %%r10\n"
        "syscall\n"::
    );
}

#define PROT_READ 0x01
#define PROT_WRITE 0x02
#define PROT_EXEC 0x04

#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#define MAP_FIXED 0x10

static void* s_mmap(void* addr, uint64_t len, uint64_t prot, uint64_t flags, uint64_t fd, uint64_t pgoff) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $9, %%al\n"
        "movq   %%rcx, %%r10\n"
        "syscall\n"::
    );
}

static int s_mprotect(void* addr, uint64_t len, uint64_t prot) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $10, %%al\n"
        "syscall\n"::
    );
}

static int s_unmap(void* addr, uint64_t len) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $11, %%al\n"
        "syscall\n"::
    );
}

#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02

static int s_open(const char* fname, uint64_t flags) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $2, %%al\n"
        "syscall\n"::
    );
}

static uint64_t s_fstat(int fd, const void* stat) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $5, %%al\n"
        "syscall\n"::
    );
}

static uint64_t s_close(int fd) {
    asm(
        "xor    %%rax, %%rax\n"
        "movb   $3, %%al\n"
        "syscall\n"::
    );
}

//uint64_t s_lseek(int fd, int off, int whence) {
//    asm(
//        "xor    %%rax, %%rax\n"
//        "movb   $8, %%al\n"
//        "syscall\n"::
//    );
//}

#pragma GCC pop_options

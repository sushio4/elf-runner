#pragma once
#include <stdint.h>
#include <elf.h>
#define nullptr ((void*)0)
#define STDOUT 1
#define STDIN 0
#define STDERR 2


static int strlen(const char * str);
static int puts(const char * str);
static void hexprint(uint64_t val);

static void* mmap_above(void* addr, uint64_t size, uint64_t prot, uint64_t flags, int fd, uint64_t pgoff);
static uint64_t fsize(int fd);
static void copy_chunk(char* restrict dest, const char* restrict src, uint64_t size);

static void load_program(Elf64_Phdr* phdr, int phnum, char* file);
#include <elf.h>

void load_code_start(void) {}

#include "utils_loaded_def.h"

void main_loaded(int argc, char** argv, void* free_chunk) {
    #ifdef DEBUG
    prnt("Running from loaded chunk...\n");
    #endif

    int fd = s_open(argv[1], O_RDONLY);
    
    if(fd <= 0) {
        prnt("Could not open file :c\n");
        s_exit(1);
    }
    
    int fsz = fsize(fd);
    char* file = mmap_above(free_chunk, fsz, PROT_READ, MAP_PRIVATE, fd, 0);
    
    if(file == nullptr) {
        prnt("Could not load file :/\n");
        s_exit(1);
    }

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)file;
    Elf64_Phdr* phdr = (Elf64_Phdr*)(file + ehdr->e_phoff);

    load_program(phdr, ehdr->e_phnum, file);

    s_unmap(free_chunk, fsz);
    s_close(fd);

    #ifdef DEBUG
    char str[] = "Trying to run the program...\n-------------------------------------\n\n";
    puts(str);
    #endif

    void (*entry)(void) = (void (*)(void))(ehdr->e_entry);

    argv[0] = (char*)(uint64_t)(argc-1);
    
    asm (
        "mov    %0, %%rax\n"
        "mov    %1, %%rsp\n"
        "xor    %%rbx, %%rbx\n"
        "xor    %%rcx, %%rcx\n"
        "xor    %%rsi, %%rsi\n"
        "xor    %%rdi, %%rdi\n"
        "xor    %%rbp, %%rbp\n"
        "xor    %%r8, %%r8\n"
        "xor    %%r9, %%r9\n"
        "xor    %%r10, %%r10\n"
        "xor    %%r11, %%r11\n"
        "xor    %%r12, %%r12\n"
        "xor    %%r13, %%r13\n"
        "xor    %%r14, %%r14\n"
        "xor    %%r15, %%r15\n"
        "push   %%rax\n"
        "xor    %%rax, %%rax\n"
        "ret\n"
        :: "rm" (entry), "rm" (argv)
    );

    s_exit(0);
}

void load_code_end(void) {}
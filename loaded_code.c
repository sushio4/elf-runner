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
    
    char* file = mmap_above(free_chunk, fsize(fd), PROT_READ, MAP_PRIVATE, fd, 0);
    
    if(file == nullptr) {
        prnt("Could not load file :/\n");
        s_exit(1);
    }

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)file;
    Elf64_Phdr* phdr = (Elf64_Phdr*)(file + ehdr->e_phoff);

    load_program(phdr, ehdr->e_phnum, file);

    s_unmap(free_chunk, fsize(fd));
    s_close(fd);

    #ifdef DEBUG
    char str[] = "Trying to run the program...\n-------------------------------------\n\n";
    puts(str);
    #endif

    void (*entry)(void) = (void (*)(void))(ehdr->e_entry);

    argv[0] = (char*)(uint64_t)(argc-1);
    
    asm(
        "mov    %0, %%rax\n"
        "mov    %1, %%rsp\n"
        "jmp    *%%rax"
        :: "rm" (entry), "rm" (argv)
    );

    // entry();

    s_exit(0);
}

void load_code_end(void) {}
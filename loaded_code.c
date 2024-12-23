#include <elf.h>

void load_code_start(void) {}

#include "utils_loaded_def.h"

void main_loaded(int argc, char** argv, void* free_chunk) {
    int fd = s_open(argv[1], O_RDONLY);
    char* file = mmap_above(free_chunk, fsize(fd), PROT_READ, MAP_PRIVATE, fd, 0);

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)file;
    Elf64_Phdr* phdr = (Elf64_Phdr*)(file + ehdr->e_phoff);

    load_program(phdr, ehdr->e_phnum, file);

    #ifdef DEBUG
    char str[] = "Trying to run the program...\n";
    puts(str);
    #endif

    void (*entry)(void) = (void (*)(void))(ehdr->e_entry);
    
    entry();

    s_exit(0);
}

void load_code_end(void) {}
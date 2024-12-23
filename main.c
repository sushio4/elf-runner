#include "syscalls.h"
#include "tinylib.h"
// #include "utils.h"
#include <elf.h>

#include "loaded_code.h"

int _runner_start(int argc, char** argv) {
    if(argc < 2) {
        t_puts("Too little arguments!\nUsage: elf-runner PROGRAM ARGS...\n");
        return 1;        
    }
    int fd = s_open(argv[1], O_RDONLY);
    uint64_t file_size = t_fsize(fd);
    char* file = s_mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)file;

    if(*(uint32_t*)file != 0x464C4574) {
        t_puts("File is not an ELF!\n");
        return 2;
    }
    
    if(file[4] != ELFCLASS64) {
        t_puts("Executable is not 64 bit!\n");
        return 3;
    }

    Elf64_Phdr* phdr = (Elf64_Phdr*)(file + ehdr->e_phoff);

    //load the code to the memory the program won't use
    //then execute from there
    void* free_memory = find_unused_memory(phdr, ehdr->e_phnum);

    uint64_t load_code_size = (char*)&load_code_end - (char*)&load_code_start;
    void* loaded_code = mmap_above(free_memory, load_code_size);
    copy_chunk(loaded_code, (char*)&load_code_start, load_code_size);

    void (*code_ptr)(int argc, char** argv);
    uint64_t offset = (uint64_t)((char*)main_loaded - (char*)load_code_start);
    code_ptr = (void (*)(int, char**))((char*)loaded_code + offset);

    code_ptr(argc, argv);
    return 0;
}

void _start(int argc, char** argv) {
    s_exit(_runner_start(argc, argv));
}

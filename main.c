#include "syscalls.h"
#include "tinylib.h"
#include "utils.h"
#include <elf.h>

#include "loaded_code.h"

int _runner_start(int argc, char** argv) {
    if(argc < 2) {
        t_puts("Too little arguments!\nUsage: elf-runner PROGRAM ARGS...\n");
        return 1;        
    }
    int fd = s_open(argv[1], O_RDONLY);

    if(fd < 0) {
        t_puts("File does not exist!\n");
        return 1;
    }

    uint64_t file_size = t_fsize(fd);
    char* file = s_mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)file;

    if(*(uint32_t*)file != 0x464C457f) {
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

    s_unmap(file, file_size);
    s_close(fd);

    #ifdef DEBUG
    t_puts("free memory available at: ");
    t_hexprint((uint64_t)free_memory);
    t_puts("\n");
    #endif

    uint64_t load_code_size = (char*)&load_code_end - (char*)&load_code_start;
    void* loaded_code = mmap_above(free_memory, load_code_size);

    if(loaded_code == nullptr) {
        t_puts("Could not allocate memory there! ToT \n");
        s_exit(1);
    }

    copy_chunk(loaded_code, (char*)&load_code_start, load_code_size);

    void* free_chunk = (char*)loaded_code + load_code_size;

    void (*loaded_entry)(int, char**, void*);
    uint64_t offset = (uint64_t)((char*)main_loaded - (char*)load_code_start);
    loaded_entry = (void (*)(int, char**, void*))((char*)loaded_code + offset);

    #ifdef DEBUG
    t_puts("Loaded code addr: ");
    t_hexprint((uint64_t)loaded_code);
    t_puts("\n");
    #endif

    loaded_entry(argc, argv, free_chunk);
    return 0;
}

void _prog_start() {
    uint64_t* sp = nullptr;
    asm( "mov %%rbp, %0" : "=rm" (sp) );
    //inc because old rbp is on the stack
    sp++;
    s_exit(_runner_start(*sp, (char**)(sp+1)));
}

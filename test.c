#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinylib.h"
#include "utils.h"

#define ASSERT(condition) {if(!(condition)) {printf("\nError on line %d: %s expected to be true!\n", __LINE__, # condition); exit(1);}}
#define ASSERT_NOT(condition) {if((condition)) {printf("\nError on line %d: %s expected to be false!\n", __LINE__, # condition ); exit(1);}}
#define ASSERT_EQ(x, y) {if((x) != (y)) {printf("\nError on line %d: %s and %s expected to be equal!\n", __LINE__, # x , # y ); exit(1);}}
#define ASSERT_NEQ(x, y) {if((x) == (y)) {printf("\nError on line %d: %s and %s expected not to be equal!\n", __LINE__, # x , # y ); exit(1);}}

void test_strlen() {
    printf("Testing strlen... ");

    ASSERT_EQ(strlen("ABC"), t_strlen("ABC"));
    ASSERT_EQ(strlen(""), t_strlen(""));
    ASSERT_NEQ(strlen("ABC"), t_strlen("ABCDEF"));
    ASSERT_EQ(t_strlen(nullptr), -1);

    printf("[OK]\n");
}

void test_tinylib() {
    printf("Testing tinylib:\n");
    test_strlen();
}


void test_mmap_above() {
    printf("Testing mmap_above... ");

    ASSERT((char*)mmap_above(nullptr, 10) > (char*)0);
    void* ptr;
    ASSERT_NEQ(ptr = mmap_above((void*)0x2000000, 0x400), nullptr);
    s_unmap(ptr, 0x400);

    ptr = mmap_above((void*)0x1000000, 0x400);
    ASSERT((uint64_t)ptr >= 0x1000000);
    s_unmap(ptr, 0x400);

    // ptr = mmap_above((void*)0x10000001, 0x1000);
    // ASSERT((uint64_t)ptr >= 0x10000001);
    // s_unmap(ptr, 0x1000);

    printf("[OK]\n");
}

void test_find_unused_memory() {
    printf("Testing find_unused_memory... ");

    Elf64_Phdr phdr[3];
    phdr[0].p_type = phdr[1].p_type = phdr[2].p_type = PT_LOAD;
    phdr[0].p_vaddr = 0x1000;   phdr[0].p_memsz = 0x4000;
    phdr[1].p_vaddr = 0x10000;  phdr[1].p_memsz = 0x200; //highest
    phdr[2].p_vaddr = 0x6000;   phdr[2].p_memsz = 0x1000;
    ASSERT_EQ((uint64_t)find_unused_memory(phdr, 3), 0x10200);

    printf("[OK]\n");
}

void test_copy_chunk() {
    printf("Testing copy_chunk... ");

    char* src = "0000111122223333000011112222333300001111222233330000111122223333";
    char dest[65];
    copy_chunk(dest, src, 65);
    ASSERT_EQ(strcmp(src, dest), 0);

    printf("[OK]\n");
}

void test_utils() {
    printf("Testing utils:\n");
    test_mmap_above();
    test_find_unused_memory();
    test_copy_chunk();
}


int main() {
    test_tinylib();
    test_utils();
    printf("Testing complete without errors\n");
    return 0;
}
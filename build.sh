gcc main.c loaded_code.c -o elf-runner -nostdlib --entry=_prog_start -fPIC -fno-inline -fno-reorder-blocks -O0 -DDEBUG -Wno-builtin-declaration-mismatch
gcc test.c -o test-elf-runner
gcc test_program/test.c -o test_program/test -nostdlib
gcc test_program/test2.c -o test_program/test2 -static -static-libgcc
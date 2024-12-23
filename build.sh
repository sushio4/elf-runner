gcc main.c loaded_code.c -o elf-runner -nostdlib --entry=_prog_start -fPIC -fno-inline -fno-reorder-blocks -O0
gcc test.c -o test-elf-runner
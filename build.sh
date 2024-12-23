gcc main.c -o elf-runner -nostdlib --entry=_start
gcc test.c -o test-elf-runner
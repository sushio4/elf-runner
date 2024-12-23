void load_code_start(void) {}

#include "syscalls.h"

void main_loaded(int argc, char** argv) {
    char str[] = "Hello from loaded chunk of code!\n";
    s_write(1, str, 33);
    s_exit(0);
}

void load_code_end(void) {}
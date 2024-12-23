#include "../syscalls.h"

void _start() {
	s_write(1, "Hello, world!\n", 14);
	s_exit(0);
}

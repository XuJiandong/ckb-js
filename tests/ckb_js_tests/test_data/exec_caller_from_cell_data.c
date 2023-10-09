#include "ckb_syscalls.h"

int main() {
    int argc = 1;
    char *argv[] = {"-f"};
    syscall(2043, 1, 3, 0, 0, argc, argv);
    return -1;
}

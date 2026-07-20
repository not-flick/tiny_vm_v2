
#include "stdlib.h"
#include "stdio.h"
#include <stdint.h>


#include "../console/console.h"
#include "kernel.h"
#include "tvm_memory.h"


ConsoleHandle* console = NULL;

void boot(void)
{
    Memory* vm_memory = memory_create(256 * 1024 * 1024);

    if (!vm_memory) {
        printf("Failed to allocate VM memory.\n");
        return;
    }

    if (!console) {
        console = console_create(1280, 720, "TinyVM Kernel");
    }

    if (!console) {
        memory_destroy(vm_memory);
        printf("Failed to create console.\n");
        return;
    }

    console_write(console,
        "\x1b[32m[OK]\x1b[0m Virtual RAM created (256 MiB)\n");

    // Continue booting...
}
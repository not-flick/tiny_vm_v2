
#include "stdlib.h"
#include "stdio.h"
#include <stdint.h>


#include "../console/console.h"
#include "kernel.h"

#define KB(x) ((x) * 1024u)
#define MB(x) (KB(x) * 1024u)
#define GB(x) (MB(x) * 1024u)

#define VM_RAM_SIZE MB(256)

ConsoleHandle* console = NULL;

static uint8_t vm_ram[VM_RAM_SIZE];
void boot() {
    if (!console) {
        console = console_create(1280, 720, "TinyVM Kernel");
    }

    console_write(console, "\x1b[32m[OK]\x1b[0m Virtual RAM created (256 MiB)\n");
}
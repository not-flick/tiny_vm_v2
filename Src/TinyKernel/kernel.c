#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../console/console.h"

#include "kernel.h"
#include "ramauto.h"
#include "tvm_memory.h"

ConsoleHandle* console = NULL;

void boot(void)
{
    /*--------------------------------------------------*/
    /* Create Virtual Memory                            */
    /*--------------------------------------------------*/

    uint64_t vm_ram = ramauto_resolve(TVM_RAM_AUTO);

    Memory* vm_memory = memory_create(vm_ram);

    if (!vm_memory)
    {
        printf("Failed to allocate VM memory.\n");
        return;
    }

    /*--------------------------------------------------*/
    /* Create Console                                   */
    /*--------------------------------------------------*/

    if (!console)
    {
        console = console_create(
            1280,
            720,
            "TinyVM Kernel"
        );
    }

    if (!console)
    {
        memory_destroy(vm_memory);

        printf("Failed to create console.\n");
        return;
    }

    /*--------------------------------------------------*/
    /* Boot Messages                                    */
    /*--------------------------------------------------*/

    char buffer[128];

    snprintf(
        buffer,
        sizeof(buffer),
        "\x1b[32m[OK]\x1b[0m Virtual RAM created (%s)\n",
        ramauto_to_string(vm_ram)
    );

    console_write(console, buffer);

    // Continue booting...

    /*
        TODO:
        - Initialize CPU
        - Initialize Registers
        - Initialize Process Manager
        - Initialize Scheduler
        - Initialize File System
        - Initialize Syscall Table
        - Launch TinyShell
    */

    /*
        Don't destroy vm_memory here.
        It should remain allocated until TinyVM exits.
    */
}
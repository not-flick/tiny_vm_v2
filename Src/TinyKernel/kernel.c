#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../Platform/fileio.h"

#include "../console/console.h"

#include "kernel.h"
#include "ramauto.h"
#include "tvm_memory.h"
#include "page.h"

ConsoleHandle* console = NULL;
uint64_t current_virtual_ram = 0;

Memory *vm_memory = NULL;
void boot(void)
{
    /*--------------------------------------------------*/
    /* Create Virtual Memory                            */
    /*--------------------------------------------------*/

    uint64_t requested_ram = TVM_RAM_AUTO;
    size_t conf_size = 0;
    unsigned char* conf_data = fileio_read("tinyvm.conf", &conf_size);
    if (conf_data) {
        if (conf_size > 0) {
            char* str = (char*)conf_data;
            if (strncmp(str, "auto", 4) != 0) {
                uint64_t parsed = strtoull(str, NULL, 10);
                if (parsed > 0) {
                    requested_ram = parsed;
                }
            }
        }
        free(conf_data);
    }

    uint64_t vm_ram = ramauto_resolve(requested_ram);
    current_virtual_ram = vm_ram;

    
    vm_memory = memory_create(vm_ram);

    Page *pages = getpages(vm_memory);

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

    if (!pages)
    {
        snprintf(buffer, sizeof(buffer),
            "\x1b[31m[FAIL]\x1b[0m Failed to get pages from virtual memory.\n");
    }
    else
    {
        snprintf(buffer, sizeof(buffer),
            "\x1b[32m[OK]\x1b[0m Pages retrieved from virtual memory.\n");
    }

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
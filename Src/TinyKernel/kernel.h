#pragma once

#include "../console/console.h"
#include "tvm_memory.h"


#ifdef __cplusplus
extern "C" {
#endif

extern ConsoleHandle* console;
extern uint64_t current_virtual_ram;
extern Memory* vm_memory;
void boot();

#ifdef __cplusplus
}
#endif
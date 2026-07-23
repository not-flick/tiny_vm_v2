#pragma once

#include "../console/console.h"


#ifdef __cplusplus
extern "C" {
#endif

extern ConsoleHandle* console;
extern uint64_t current_virtual_ram;
void boot();

#ifdef __cplusplus
}
#endif
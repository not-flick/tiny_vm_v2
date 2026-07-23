#pragma once

#include <stddef.h>
#include "tvm_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    size_t offset;
} Page;

Page *getpages(Memory *memory);
void freepages(Page *pages);

#ifdef __cplusplus
}
#endif
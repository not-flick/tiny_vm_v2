#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Memory Memory;

Memory* memory_create(size_t size);
void memory_destroy(Memory* memory);

size_t memory_size(const Memory* memory);

uint8_t  memory_read8 (const Memory* memory, size_t addr);
uint16_t memory_read16(const Memory* memory, size_t addr);
uint32_t memory_read32(const Memory* memory, size_t addr);
uint64_t memory_read64(const Memory* memory, size_t addr);

void memory_write8 (Memory* memory, size_t addr, uint8_t value);
void memory_write16(Memory* memory, size_t addr, uint16_t value);
void memory_write32(Memory* memory, size_t addr, uint32_t value);
void memory_write64(Memory* memory, size_t addr, uint64_t value);

void memory_reset(Memory* memory);
void memory_fill(Memory* memory, uint8_t value);
void memory_copy(Memory* memory,
                 size_t dst,
                 size_t src,
                 size_t len);

#ifdef __cplusplus
}
#endif
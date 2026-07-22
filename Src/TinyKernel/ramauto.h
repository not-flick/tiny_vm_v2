#ifndef RAMAUTO_H
#define RAMAUTO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KiB (1024ULL)
#define MiB (1024ULL * KiB)
#define GiB (1024ULL * MiB)

#define TVM_RAM_AUTO 0ULL

#define TVM_MIN_RAM (128ULL * MiB)
#define TVM_MAX_RAM (4ULL * GiB)

/*
 * Returns the total amount of physical RAM installed
 * in the host computer.
 */
uint64_t ramauto_host_ram(void);

/*
 * Resolves the amount of RAM TinyVM should allocate.
 *
 * Pass TVM_RAM_AUTO to automatically detect RAM.
 * Otherwise pass the desired RAM size in bytes.
 */
uint64_t ramauto_resolve(uint64_t requested_ram);

/*
 * Converts a RAM size into a readable string.
 *
 * Example:
 * 134217728  -> "128 MiB"
 * 1073741824 -> "1 GiB"
 */
const char* ramauto_to_string(uint64_t bytes);

#ifdef __cplusplus
}
#endif

#endif /* RAMAUTO_H */
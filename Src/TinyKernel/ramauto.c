#include "ramauto.h"

#include <stdio.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif defined(__APPLE__)

#include <sys/sysctl.h>

#else

#include <unistd.h>

#endif

/*----------------------------------------------------------*/
/* Internal Helpers                                         */
/*----------------------------------------------------------*/

static uint64_t clamp_ram(uint64_t ram)
{
    if (ram < TVM_MIN_RAM)
        return TVM_MIN_RAM;

    if (ram > TVM_MAX_RAM)
        return TVM_MAX_RAM;

    return ram;
}

static uint64_t nearest_power_of_two(uint64_t value)
{
    if (value <= 1)
        return 1;

    uint64_t lower = 1;

    while ((lower << 1) <= value)
        lower <<= 1;

    uint64_t upper = lower << 1;

    if ((value - lower) < (upper - value))
        return lower;

    return upper;
}

/*----------------------------------------------------------*/
/* Public API                                               */
/*----------------------------------------------------------*/

uint64_t ramauto_host_ram(void)
{

#ifdef _WIN32

    MEMORYSTATUSEX memory;

    memory.dwLength = sizeof(memory);

    if (GlobalMemoryStatusEx(&memory))
        return memory.ullTotalPhys;

    return TVM_MIN_RAM;

#elif defined(__APPLE__)

    uint64_t ram = 0;

    size_t size = sizeof(ram);

    if (sysctlbyname("hw.memsize", &ram, &size, NULL, 0) == 0)
        return ram;

    return TVM_MIN_RAM;

#else

    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);

    if (pages <= 0 || page_size <= 0)
        return TVM_MIN_RAM;

    return (uint64_t)pages * (uint64_t)page_size;

#endif
}

uint64_t ramauto_resolve(uint64_t requested_ram)
{
    uint64_t ram;

    if (requested_ram == TVM_RAM_AUTO)
    {
        ram = ramauto_host_ram();

        ram /= 64;
    }
    else
    {
        ram = requested_ram;
    }

    ram = clamp_ram(ram);

    ram = nearest_power_of_two(ram);

    ram = clamp_ram(ram);

    return ram;
}

const char* ramauto_to_string(uint64_t bytes)
{
    static char buffer[32];

    if (bytes >= GiB)
    {
        double value = (double)bytes / (double)GiB;

        if ((uint64_t)bytes % GiB == 0)
            snprintf(buffer, sizeof(buffer), "%.0f GiB", value);
        else
            snprintf(buffer, sizeof(buffer), "%.2f GiB", value);
    }
    else
    {
        double value = (double)bytes / (double)MiB;

        if ((uint64_t)bytes % MiB == 0)
            snprintf(buffer, sizeof(buffer), "%.0f MiB", value);
        else
            snprintf(buffer, sizeof(buffer), "%.2f MiB", value);
    }

    return buffer;
}
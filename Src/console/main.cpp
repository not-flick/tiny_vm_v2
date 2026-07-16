#include "console.h"
#include "../TinyKernel/kernel.h"

int main()
{
    // Initialize the kernel which creates the console
    boot();

    // Run the event loop using the console created by boot()
    if (console)
    {
        while (console_is_open(console))
        {
            console_poll_events(console);
            console_present(console);
        }
    }

    return 0;
}
#include "console.h"
#include "../TinyKernel/kernel.h"
#include "BootLogo.h"
#include "../Platform/platform.h"
#include <SDL3/SDL.h>
#include "../shell/shell.h"

int main()
{
    // Initialize the kernel which creates the console
    boot();

    if (console)
    {
        Console* cpp_console = reinterpret_cast<Console*>(console);
        
        // Wait ~500ms after boot log to keep it visible
        uint64_t start = SDL_GetTicks();
        while (SDL_GetTicks() - start < 500 && console_is_open(console)) {
            console_poll_events(console);
            console_present(console);
        }

        // Show Boot Logo if available
        BootLogo logo;
        auto logoPath = Platform::assetsDirectory() / "boot/logo.png";


        
        if (logo.load(cpp_console->getRenderer(), logoPath)) {
            console_clear(console);
            logo.show(cpp_console->getRenderer(), cpp_console->getWindow(), 2000);
            console_clear(console);
        }

        // while (true){
        //         shell();
        // }

        // Run the main event loop
        while (console_is_open(console))
        {
            console_poll_events(console);
            console_present(console);

        }
    }

    shell();


    return 0;
}
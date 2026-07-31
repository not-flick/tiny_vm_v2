#include "console.h"
#include "../TinyKernel/kernel.h"
#include "BootLogo.h"
#include "../Platform/platform.h"
#include <SDL3/SDL.h>
#include "../shell/shell.h"
#include "../shell/ShellSystem.h"
#include "../TinyKernel/ramauto.h"

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

        // ---- Boot banner is now part of the scrollback history ----
        // Written as a true renderable image entry.
        auto bannerPath = Platform::assetsDirectory() / "banner/banner.png";
        console_write_image(console, bannerPath.string().c_str());
        
        console_write_line(console, "\x1b[37mTinyVM v0.1  —  TVM_64 Architecture\x1b[0m");
        console_write_line(console, "");
        std::string hostRamMsg = "Host RAM    : " + std::string(ramauto_to_string(ramauto_host_ram()));
        std::string vmRamMsg   = "Virtual RAM : " + std::string(ramauto_to_string(current_virtual_ram));
        console_write_line(console, hostRamMsg.c_str());
        console_write_line(console, vmRamMsg.c_str());
        console_write_line(console, "");


        ShellSystem shell_sys;

        // Run the main event loop
        while (console_is_open(console))
        {
            console_poll_events(console);
            shell_sys.update();
            console_present(console);

        }
    }

    return 0;
}
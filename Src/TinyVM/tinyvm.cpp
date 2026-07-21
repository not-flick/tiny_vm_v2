#include <iostream>
#include <string>

#include "../Platform/fileio.h"
#include "../Platform/platform.h"
#include "../TinyKernel/kernel.h"
#include "../TinyKernel/process.h"


std::string username;


int main(){
    std::cout << "enter username: ";
    std::cin >> username;

    std::string os = Platform::name();

    if (os == "Windows" || os == "Linux" || os == "macOS") {

        std::string root = Platform::rootDirectory().string() + "/tinyvm";

        fileio_mkdir(root.c_str());

        // Root directories
        fileio_mkdir((root + "/app").c_str());
        fileio_mkdir((root + "/boot").c_str());
        fileio_mkdir((root + "/config").c_str());
        fileio_mkdir((root + "/data").c_str());
        fileio_mkdir((root + "/device").c_str());
        fileio_mkdir((root + "/libs").c_str());
        fileio_mkdir((root + "/logs").c_str());
        fileio_mkdir((root + "/mounts").c_str());
        fileio_mkdir((root + "/runtime").c_str());
        fileio_mkdir((root + "/services").c_str());
        fileio_mkdir((root + "/system").c_str());
        fileio_mkdir((root + "/temp").c_str());
        fileio_mkdir((root + "/users").c_str());
        fileio_mkdir((root + "/vm").c_str());

        // User
        std::string home = root + "/users/" + username;

        fileio_mkdir(home.c_str());

        fileio_mkdir((home + "/Desktop").c_str());
        fileio_mkdir((home + "/Documents").c_str());
        fileio_mkdir((home + "/Downloads").c_str());
        fileio_mkdir((home + "/Music").c_str());
        fileio_mkdir((home + "/Pictures").c_str());
        fileio_mkdir((home + "/Videos").c_str());
        fileio_mkdir((home + "/Projects").c_str());
        fileio_mkdir((home + "/Workspace").c_str());
        fileio_mkdir((home + "/.config").c_str());
        fileio_mkdir((home + "/.cache").c_str());
        fileio_mkdir((home + "/.local").c_str());

        // Boot
        fileio_mkdir((root + "/boot/kernel").c_str());
        fileio_mkdir((root + "/boot/themes").c_str());

        // Config
        fileio_mkdir((root + "/config/system").c_str());
        fileio_mkdir((root + "/config/users").c_str());

        // Apps
        fileio_mkdir((root + "/apps/system").c_str());

        // Libraries
        fileio_mkdir((root + "/libs/system").c_str());

        // Logs
        fileio_mkdir((root + "/logs/kernel").c_str());
        fileio_mkdir((root + "/logs/shell").c_str());

        // Runtime
        fileio_mkdir((root + "/runtime/pids").c_str());
        fileio_mkdir((root + "/runtime/sockets").c_str());

        // VM
        fileio_mkdir((root + "/vm/cpu").c_str());
        fileio_mkdir((root + "/vm/memory").c_str());
        fileio_mkdir((root + "/vm/devices").c_str());
        fileio_mkdir((root + "/vm/processes").c_str());

        // Mounts
        fileio_mkdir((root + "/mounts/disks").c_str());
        fileio_mkdir((root + "/mounts/usb").c_str());

        // Temp
        fileio_mkdir((root + "/temp/cache").c_str());

        // Launch the console
        process_init();
        process_create("TinyConsole");
    }

    else{
        std::cout << "Unsupported operating system: " << os << std::endl;
    }

    return 0;
}
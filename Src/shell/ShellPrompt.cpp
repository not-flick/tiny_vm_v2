#include "ShellPrompt.h"
#include "../console/console.h"
#include "../TinyKernel/kernel.h"
#include "../TinyVM/tinyvm.h"
#include "../Platform/platform.h"

ShellPrompt::ShellPrompt() {
    hostname = "tinyvm"; // Use "tinyvm" instead of default host OS
    extern std::string username;
    currentDir = "/users/" + username;
}

void ShellPrompt::setCurrentDir(const std::string& dir) {
    currentDir = dir;
}

std::string ShellPrompt::getCurrentDir() const {
    return currentDir;
}

void ShellPrompt::display() {
    extern std::string username;
    std::string vhome = "/users/" + username;
    std::string displayPath = currentDir;
    
    // Replace home directory with ~
    if (displayPath.find(vhome) == 0) {
        displayPath.replace(0, vhome.length(), "~");
    }
    
    // Format: username@hostname:path$ 
    // Username (Green 32), @ (White 37), Hostname (Cyan 36), : (White 37), Directory (Blue 34), $ (White 37)
    std::string promptStr = "\x1b[32m" + username + "\x1b[37m@\x1b[36m" + hostname + "\x1b[37m:\x1b[34m" + displayPath + "\x1b[37m$ \x1b[0m";
    
    console_write(console, promptStr.c_str());
}

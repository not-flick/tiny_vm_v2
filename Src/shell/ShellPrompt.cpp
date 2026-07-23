#include "ShellPrompt.h"
#include "../console/console.h"
#include "../TinyKernel/kernel.h"
#include "../TinyVM/tinyvm.h"
#include "../Platform/platform.h"

ShellPrompt::ShellPrompt() {
    hostname = "fedora"; // Default hostname
    currentDir = Platform::homeDirectory().string();
}

void ShellPrompt::setCurrentDir(const std::string& dir) {
    currentDir = dir;
}

std::string ShellPrompt::getCurrentDir() const {
    return currentDir;
}

void ShellPrompt::display() {
    std::string home = Platform::homeDirectory().string();
    std::string displayPath = currentDir;
    
    // Replace home directory with ~
    if (displayPath.find(home) == 0) {
        displayPath.replace(0, home.length(), "~");
    }
    
    // Format: username@hostname:path$ 
    // Using ANSI escape codes for coloring (green for user@host, blue for path)
    std::string promptStr = "\x1b[32m" + username + "@" + hostname + "\x1b[0m:\x1b[34m" + displayPath + "\x1b[0m$ ";
    
    console_write(console, promptStr.c_str());
}

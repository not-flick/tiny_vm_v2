#pragma once
#include <string>

class ShellPrompt {
public:
    ShellPrompt();
    void display();
    void setCurrentDir(const std::string& dir);
    std::string getCurrentDir() const;
private:
    std::string currentDir;
    std::string hostname;
};

#pragma once
#include <string>

class Input {
public:
    void update();
    bool hasLine() const;
    std::string getLine();
private:
    std::string bufferedLine;
    bool lineReady = false;
};

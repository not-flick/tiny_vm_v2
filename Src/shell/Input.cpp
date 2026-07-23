#include "Input.h"
#include "../console/console.h"
#include "../TinyKernel/kernel.h"
#include <cstdlib>

void Input::update() {
    if (lineReady) return;
    char* line = console_read_line(console);
    if (line) {
        if (line[0] != '\0') {
            bufferedLine = line;
            lineReady = true;
        }
        free(line);
    }
}

bool Input::hasLine() const { return lineReady; }

std::string Input::getLine() {
    lineReady = false;
    std::string res = bufferedLine;
    bufferedLine.clear();
    return res;
}

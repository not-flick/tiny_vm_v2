#include "History.h"

void History::add(const std::string& command) {
    if (!command.empty()) {
        commands.push_back(command);
    }
}

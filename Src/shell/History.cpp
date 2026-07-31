#include "History.h"
#include <algorithm>

void History::add(const std::string& command) {
    if (command.empty()) return;
    // Skip consecutive duplicate entries.
    if (!commands.empty() && commands.back() == command) return;
    commands.push_back(command);
    // Whenever a new command is added the cursor resets.
    resetCursor();
}

std::string History::navigate(int delta) {
    if (commands.empty()) return "";

    const std::size_t livePos = commands.size(); // one past the last entry

    if (delta < 0) {
        // Up arrow: move toward older entries (lower index)
        if (cursor == 0) return commands[0]; // already at oldest
        --cursor;
    } else if (delta > 0) {
        // Down arrow: move toward newer / live position
        if (cursor >= livePos) return ""; // already at live
        ++cursor;
    }

    if (cursor >= livePos) return ""; // back to live input
    return commands[cursor];
}

void History::resetCursor() {
    cursor = commands.size(); // point past-the-end → live position
}

#pragma once
#include <string>
#include <vector>

// ============================================================
// History  –  Command history with navigation cursor
// ============================================================
// Supports:
//   - Adding commands (skips consecutive duplicates)
//   - Navigating up (older) / down (newer) with arrow keys
//   - Resetting cursor to the "live" position
// ============================================================

class History {
public:
    // Add a command. Consecutive duplicates are silently dropped.
    void add(const std::string& command);

    // Navigate in history.
    //   navigate(-1)  → one step older  (Up arrow)
    //   navigate(+1)  → one step newer  (Down arrow)
    // Returns the entry at the new position, or "" when past the end
    // (i.e. the user navigated back to the live input position).
    std::string navigate(int delta);

    // Reset the navigation cursor back to the live position.
    // Call this whenever the user submits a command.
    void resetCursor();

    // Returns all history entries (oldest first) for the `history` command.
    const std::vector<std::string>& entries() const { return commands; }

private:
    std::vector<std::string> commands;
    // cursor == commands.size() → at the live position (no recall active)
    std::size_t cursor = 0;
};

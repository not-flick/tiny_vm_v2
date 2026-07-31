#pragma once
// ============================================================
// Completion  –  Tab completion for TinyShell
// ============================================================
// Completes:
//   - Command names (from a static registry)
//   - Files and directories relative to the given cwd
//
// The Console is kept completely unaware of this class.
// ============================================================

#include <string>
#include <vector>

class Completion {
public:
    // Return all completions for the given prefix.
    // `cwd` is the current working directory for path completion.
    std::vector<std::string> complete(const std::string& prefix,
                                     const std::string& cwd) const;

    // All known command names (used for command completion).
    static const std::vector<std::string>& commandNames();
};

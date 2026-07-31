#include "Completion.h"
#include <filesystem>
#include <algorithm>

// ============================================================
// Command name registry
// ============================================================

const std::vector<std::string>& Completion::commandNames() {
    static const std::vector<std::string> names = {
        // Filesystem
        "ls", "cd", "pwd", "mkdir", "touch", "rm", "rmdir",
        "cp", "mv", "cat", "find", "tree",
        // Environment
        "env", "export", "set", "unset",
        // Utilities
        "echo", "head", "tail", "wc", "which", "whereis",
        // Shell
        "clear", "history", "help", "exit",
        // System
        "version", "whoami", "hostname", "setram", "ram", "run", "reboot",
    };
    return names;
}

// ============================================================
// Completion::complete()
// ============================================================

std::vector<std::string> Completion::complete(const std::string& prefix,
                                              const std::string& cwd) const
{
    std::vector<std::string> results;
    if (prefix.empty()) return results;

    // ---- Decide: are we completing a command or a path? ----
    // If the prefix contains a '/' or starts with './' or '../', treat
    // it as a path.  Otherwise try commands first, then fall through to paths.

    bool hasSlash = prefix.find('/') != std::string::npos
                 || prefix.substr(0, 2) == "./"
                 || prefix.substr(0, 3) == "../";

    if (!hasSlash) {
        // ---- Command completion ----
        for (const auto& cmd : commandNames()) {
            if (cmd.size() >= prefix.size() &&
                cmd.substr(0, prefix.size()) == prefix)
            {
                results.push_back(cmd);
            }
        }
        if (!results.empty()) return results;
        // No command matched → fall through to path completion.
    }

    // ---- Path / file completion ----
    // Split prefix into directory part and name part.
    std::string dirPart, namePart;
    auto slashPos = prefix.rfind('/');
    if (slashPos == std::string::npos) {
        dirPart  = "";
        namePart = prefix;
    } else {
        dirPart  = prefix.substr(0, slashPos + 1);
        namePart = prefix.substr(slashPos + 1);
    }

    std::filesystem::path searchDir = cwd;
    if (!dirPart.empty()) {
        if (dirPart[0] == '/')
            searchDir = dirPart; // absolute
        else
            searchDir = std::filesystem::path(cwd) / dirPart;
    }

    std::error_code ec;
    std::filesystem::directory_iterator it(searchDir, ec);
    if (ec) return results;

    for (const auto& entry : it) {
        std::string name = entry.path().filename().string();
        if (name.size() >= namePart.size() &&
            name.substr(0, namePart.size()) == namePart)
        {
            std::string completion = dirPart + name;
            if (entry.is_directory(ec)) completion += '/';
            results.push_back(completion);
        }
    }
    std::sort(results.begin(), results.end());
    return results;
}

#pragma once
// ============================================================
// Executor  –  Command dispatcher for TinyShell
// ============================================================
// Commands are registered in a static dispatch table.
// Adding new commands never requires changing Executor.h —
// only a new entry in the table in Executor.cpp.
// ============================================================

#include "Parser.h"
#include "ShellPrompt.h"
#include "History.h"
#include <functional>
#include <string>
#include <unordered_map>

class Executor {
public:
    // Construct with references to the shell state objects that
    // some commands need to read or mutate.
    explicit Executor(ShellPrompt& prompt, History& history);

    // Execute a parsed command.
    void execute(const Parser::Command& cmd);

private:
    ShellPrompt& prompt;
    History&     history;

    using Handler = std::function<void(const Parser::Command&)>;
    std::unordered_map<std::string, Handler> dispatch;

    void registerCommands();

    // ---- Helpers ----

    // Resolve a path argument relative to the virtual cwd.
    // Absolute paths (starting with '/') are returned unchanged.
    std::string resolvePath(const std::string& arg) const;

    // Print a formatted error to the console.
    static void error(const char* cmd, const char* msg);
};

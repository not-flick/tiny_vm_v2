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

    // Resolve an argument into an absolute virtual path (e.g. "/users/not-flick").
    // Returns empty string if the path attempts to escape the virtual root (e.g. "/../").
    static std::string resolveVirtualPath(const std::string& arg, const std::string& cwd);

    // Map an absolute virtual path to the real host path.
    static std::string mapToHostPath(const std::string& virtualPath);

private:
    ShellPrompt& prompt;
    History&     history;

    using Handler = std::function<void(const Parser::Command&)>;
    std::unordered_map<std::string, Handler> dispatch;

    void registerCommands();

    // ---- Helpers ----

    // Print a formatted error to the console.
    static void error(const char* cmd, const char* msg);
};

#include "HelpText.h"
#include "../console/console.h"

// ============================================================
// Per-command help entries
// ============================================================

namespace {

struct CmdHelp {
    const char* description;
    const char* usage;
    const char* examples; // newline-separated
};

#define ENTRY(desc, usage, examples) { desc, usage, examples }

// clang-format off
static const std::pair<const char*, CmdHelp> helpTable[] = {
    // ---- Filesystem ----
    { "ls",      ENTRY("List directory contents.",
                       "ls [directory]",
                       "ls\nls /users/bob/Documents") },
    { "cd",      ENTRY("Change the current directory.",
                       "cd <directory>",
                       "cd Documents\ncd /\ncd ..") },
    { "pwd",     ENTRY("Print the current working directory.",
                       "pwd",
                       "pwd") },
    { "mkdir",   ENTRY("Create a new directory.",
                       "mkdir <directory>",
                       "mkdir Projects") },
    { "touch",   ENTRY("Create an empty file.",
                       "touch <file>",
                       "touch notes.txt") },
    { "rm",      ENTRY("Remove a file.",
                       "rm <file>",
                       "rm temp.txt") },
    { "rmdir",   ENTRY("Remove an empty directory.",
                       "rmdir <directory>",
                       "rmdir OldProjects") },
    { "cp",      ENTRY("Copy a file.",
                       "cp <source> <destination>",
                       "cp notes.txt notes_backup.txt") },
    { "mv",      ENTRY("Move or rename a file or directory.",
                       "mv <source> <destination>",
                       "mv old_name.txt new_name.txt\nmv file.txt /data/") },
    { "cat",     ENTRY("Print the contents of a file.",
                       "cat <file>",
                       "cat readme.txt") },
    { "head",    ENTRY("Print the first N lines of a file.",
                       "head [-n <count>] <file>",
                       "head readme.txt\nhead -n 5 log.txt") },
    { "tail",    ENTRY("Print the last N lines of a file.",
                       "tail [-n <count>] <file>",
                       "tail readme.txt\ntail -n 10 log.txt") },
    { "wc",      ENTRY("Count lines, words, and bytes in a file.",
                       "wc <file>",
                       "wc readme.txt") },
    { "find",    ENTRY("Search for files whose names match a pattern.",
                       "find [directory] <pattern>",
                       "find . txt\nfind /data log") },
    { "tree",    ENTRY("Display the directory tree.",
                       "tree [directory]",
                       "tree\ntree /users") },
    // ---- Environment ----
    { "env",     ENTRY("Print all environment variables.",
                       "env",
                       "env") },
    { "export",  ENTRY("Set or export an environment variable.",
                       "export <NAME>=<value>",
                       "export EDITOR=vim\nexport PATH=/bin") },
    { "set",     ENTRY("Set a shell variable.",
                       "set <NAME>=<value>",
                       "set GREETING=hello") },
    { "unset",   ENTRY("Unset (remove) a shell variable.",
                       "unset <NAME>",
                       "unset GREETING") },
    // ---- Utilities ----
    { "echo",    ENTRY("Print text to the console.",
                       "echo <text>",
                       "echo Hello, TinyVM!") },
    { "which",   ENTRY("Show the location of a command.",
                       "which <command>",
                       "which ls") },
    { "whereis", ENTRY("Locate a command in known paths.",
                       "whereis <command>",
                       "whereis cat") },
    { "clear",   ENTRY("Clear the console screen.",
                       "clear",
                       "clear") },
    { "history", ENTRY("Show the command history.",
                       "history",
                       "history") },
    { "help",    ENTRY("Show help for commands.",
                       "help [command]",
                       "help\nhelp cd") },
    // ---- System ----
    { "version", ENTRY("Display TinyVM version information.",
                       "version",
                       "version") },
    { "whoami",  ENTRY("Print the current username.",
                       "whoami",
                       "whoami") },
    { "hostname",ENTRY("Print the machine hostname.",
                       "hostname",
                       "hostname") },
    { "setram",  ENTRY("Configure virtual RAM size.",
                       "setram <size>[k|m|g]  or  setram auto",
                       "setram 256m\nsetram auto") },
    { "ram",     ENTRY("Display memory information.",
                       "ram",
                       "ram") },
    { "run",     ENTRY("Launch a TinyVM executable.",
                       "run <executable>",
                       "run /app/myprogram") },
    { "exit",    ENTRY("Exit TinyVM.",
                       "exit",
                       "exit") },
    { "reboot",  ENTRY("Restart TinyVM.",
                       "reboot",
                       "reboot") },
};
// clang-format on

static const char* RESET = "\x1b[0m";
static const char* CYAN  = "\x1b[36m";
static const char* GREEN = "\x1b[32m";
static const char* YELLOW= "\x1b[33m";
static const char* WHITE = "\x1b[37m";

} // anonymous namespace

// ============================================================
// Implementation
// ============================================================

namespace HelpText {

void printAll(ConsoleHandle* console)
{
    auto sec = [&](const char* title) {
        std::string line = std::string(CYAN) + title + RESET;
        console_write_line(console, line.c_str());
        std::string dash(std::string(title).size(), '-');
        console_write_line(console, (std::string(CYAN) + dash + RESET).c_str());
    };
    auto cmd = [&](const char* name) {
        console_write_line(console, (std::string("  ") + GREEN + name + RESET).c_str());
    };

    sec("Filesystem");
    cmd("ls");   cmd("cd");  cmd("pwd"); cmd("mkdir"); cmd("touch");
    cmd("cp");   cmd("mv");  cmd("rm");  cmd("rmdir"); cmd("cat");
    cmd("head"); cmd("tail");cmd("wc");  cmd("find");  cmd("tree");
    console_write_line(console, "");

    sec("Environment");
    cmd("env"); cmd("export"); cmd("set"); cmd("unset");
    console_write_line(console, "");

    sec("Utilities");
    cmd("echo"); cmd("which"); cmd("whereis"); cmd("clear"); cmd("history"); cmd("help");
    console_write_line(console, "");

    sec("System");
    cmd("version"); cmd("whoami"); cmd("hostname");
    cmd("setram"); cmd("ram"); cmd("run"); cmd("exit"); cmd("reboot");
    console_write_line(console, "");

    console_write_line(console,
        (std::string(WHITE) +
         "Type 'help <command>' for details on a specific command." + RESET).c_str());
}

bool printCommand(ConsoleHandle* console, const std::string& name)
{
    for (const auto& [cmdName, entry] : helpTable) {
        if (name == cmdName) {
            std::string header = std::string(CYAN) + cmdName + RESET
                               + "  —  " + entry.description;
            console_write_line(console, header.c_str());
            console_write_line(console, "");

            console_write_line(console,
                (std::string(YELLOW) + "Usage:" + RESET).c_str());
            console_write_line(console,
                (std::string("  ") + entry.usage).c_str());
            console_write_line(console, "");

            console_write_line(console,
                (std::string(YELLOW) + "Examples:" + RESET).c_str());

            // Split examples by newline and print each indented.
            std::string ex = entry.examples;
            std::size_t start = 0;
            while (true) {
                auto nl = ex.find('\n', start);
                std::string line = "  " + ex.substr(start, nl - start);
                console_write_line(console, line.c_str());
                if (nl == std::string::npos) break;
                start = nl + 1;
            }
            console_write_line(console, "");
            return true;
        }
    }
    return false;
}

} // namespace HelpText

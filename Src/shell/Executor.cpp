// ============================================================
// Executor.cpp  –  TinyShell command implementations
// ============================================================
#include "Executor.h"
#include "HelpText.h"
#include "../console/console.h"
#include "../TinyKernel/kernel.h"
#include "../Platform/fileio.h"
#include "../Platform/platform.h"
#include "../TinyKernel/process.h"
#include "../TinyKernel/ramauto.h"

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cassert>

// ============================================================
// Helpers
// ============================================================

static void out(const char* text) {
    console_write_line(console, text);
}

static void out(const std::string& text) {
    console_write_line(console, text.c_str());
}

void Executor::error(const char* cmd, const char* msg) {
    console_write_line(console,
        (std::string("\x1b[31m") + cmd + ": " + msg + "\x1b[0m").c_str());
}

std::string Executor::resolvePath(const std::string& arg) const {
    if (arg.empty()) return prompt.getCurrentDir();
    if (arg[0] == '/') {
        // Absolute path in the virtual FS → prepend virtual root
        std::string vroot = Platform::rootDirectory().string() + "/tinyvm";
        return vroot + arg;
    }
    // Relative path → resolve against cwd
    namespace fs = std::filesystem;
    fs::path p = fs::path(prompt.getCurrentDir()) / arg;
    // Normalise (removes ".." etc.) without checking existence
    std::error_code ec;
    auto canonical = fs::weakly_canonical(p, ec);
    return ec ? p.string() : canonical.string();
}

// ============================================================
// Constructor
// ============================================================

Executor::Executor(ShellPrompt& prompt, History& history)
    : prompt(prompt), history(history)
{
    registerCommands();
}

// ============================================================
// Command table
// ============================================================

void Executor::registerCommands()
{
    // ---- help ----
    dispatch["help"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) {
            HelpText::printAll(console);
        } else {
            if (!HelpText::printCommand(console, cmd.arguments[0])) {
                error("help", ("unknown command: " + cmd.arguments[0]).c_str());
            }
        }
    };

    // ---- clear ----
    dispatch["clear"] = [](const Parser::Command&) {
        console_clear(console);
    };

    // ---- echo ----
    dispatch["echo"] = [](const Parser::Command& cmd) {
        std::string text;
        for (size_t i = 0; i < cmd.arguments.size(); ++i) {
            text += cmd.arguments[i];
            if (i + 1 < cmd.arguments.size()) text += ' ';
        }
        out(text);
    };

    // ---- version ----
    dispatch["version"] = [](const Parser::Command&) {
        out("\x1b[36mTinyVM v0.1\x1b[0m  —  TVM_64 Architecture");
    };

    // ---- exit ----
    dispatch["exit"] = [](const Parser::Command&) {
        out("Exiting TinyVM...");
        console_close(console);
    };

    // ---- reboot ----
    dispatch["reboot"] = [](const Parser::Command&) {
        out("Rebooting TinyVM...");
        console_close(console);
    };

    // ---- whoami ----
    dispatch["whoami"] = [](const Parser::Command&) {
        // username is a global defined in tinyvm.h / tinyvm.cpp
        extern std::string username;
        out(username);
    };

    // ---- hostname ----
    dispatch["hostname"] = [](const Parser::Command&) {
        out("tinyvm");
    };

    // ---- pwd ----
    dispatch["pwd"] = [this](const Parser::Command&) {
        out(prompt.getCurrentDir());
    };

    // ---- ls ----
    dispatch["ls"] = [this](const Parser::Command& cmd) {
        std::string dir = cmd.arguments.empty()
                          ? prompt.getCurrentDir()
                          : resolvePath(cmd.arguments[0]);

        size_t count = 0;
        char** entries = fileio_list_dir(dir.c_str(), &count);
        if (!entries) {
            error("ls", ("cannot access '" + dir + "': No such directory").c_str());
            return;
        }

        // Sort alphabetically
        std::vector<std::string> names(entries, entries + count);
        fileio_list_free(entries, count);
        std::sort(names.begin(), names.end());

        std::string line;
        for (const auto& name : names) {
            std::string fullPath = dir + "/" + name;
            bool isDir = fileio_is_dir(fullPath.c_str());
            // Directories in cyan, files in white
            if (isDir)
                line += "\x1b[36m" + name + "/\x1b[0m  ";
            else
                line += name + "  ";
        }
        if (!line.empty()) out(line);
    };

    // ---- cd ----
    dispatch["cd"] = [this](const Parser::Command& cmd) {
        std::string target;
        if (cmd.arguments.empty()) {
            // cd with no arguments → virtual home directory
            target = Platform::rootDirectory().string()
                   + "/tinyvm/users/";
            extern std::string username;
            target += username;
        } else if (cmd.arguments[0] == "..") {
            namespace fs = std::filesystem;
            fs::path p(prompt.getCurrentDir());
            target = p.parent_path().string();
        } else if (cmd.arguments[0] == "/") {
            target = Platform::rootDirectory().string() + "/tinyvm";
        } else {
            target = resolvePath(cmd.arguments[0]);
        }

        if (!fileio_is_dir(target.c_str())) {
            error("cd", ("no such directory: " + cmd.arguments[0]).c_str());
            return;
        }
        prompt.setCurrentDir(target);
    };

    // ---- mkdir ----
    dispatch["mkdir"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("mkdir", "missing operand"); return; }
        std::string path = resolvePath(cmd.arguments[0]);
        if (!fileio_mkdir(path.c_str()))
            error("mkdir", ("cannot create directory '" + cmd.arguments[0] + "'").c_str());
    };

    // ---- touch ----
    dispatch["touch"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("touch", "missing operand"); return; }
        std::string path = resolvePath(cmd.arguments[0]);
        if (!fileio_write(path.c_str(), "", 0))
            error("touch", ("cannot create file '" + cmd.arguments[0] + "'").c_str());
    };

    // ---- rm ----
    dispatch["rm"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("rm", "missing operand"); return; }
        std::string path = resolvePath(cmd.arguments[0]);
        if (!fileio_delete(path.c_str()))
            error("rm", ("cannot remove '" + cmd.arguments[0] + "'").c_str());
    };

    // ---- rmdir ----
    dispatch["rmdir"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("rmdir", "missing operand"); return; }
        std::string path = resolvePath(cmd.arguments[0]);
        if (!fileio_rmdir(path.c_str()))
            error("rmdir", ("cannot remove directory '" + cmd.arguments[0] + "'").c_str());
    };

    // ---- cat ----
    dispatch["cat"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("cat", "missing operand"); return; }
        std::string path = resolvePath(cmd.arguments[0]);
        size_t size = 0;
        unsigned char* data = fileio_read(path.c_str(), &size);
        if (!data) { error("cat", ("cannot read '" + cmd.arguments[0] + "'").c_str()); return; }
        console_write(console, reinterpret_cast<char*>(data));
        console_write(console, "\n");
        free(data);
    };

    // ---- cp ----
    dispatch["cp"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.size() < 2) { error("cp", "usage: cp <source> <destination>"); return; }
        std::string src = resolvePath(cmd.arguments[0]);
        std::string dst = resolvePath(cmd.arguments[1]);
        // If dst is a directory, append the filename
        if (fileio_is_dir(dst.c_str())) {
            dst += "/" + std::filesystem::path(src).filename().string();
        }
        if (!fileio_copy(src.c_str(), dst.c_str()))
            error("cp", ("cannot copy '" + cmd.arguments[0] + "'").c_str());
    };

    // ---- mv ----
    dispatch["mv"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.size() < 2) { error("mv", "usage: mv <source> <destination>"); return; }
        std::string src = resolvePath(cmd.arguments[0]);
        std::string dst = resolvePath(cmd.arguments[1]);
        if (fileio_is_dir(dst.c_str())) {
            dst += "/" + std::filesystem::path(src).filename().string();
        }
        if (!fileio_rename(src.c_str(), dst.c_str()))
            error("mv", ("cannot move '" + cmd.arguments[0] + "'").c_str());
    };

    // ---- head ----
    dispatch["head"] = [this](const Parser::Command& cmd) {
        int n = 10;
        std::string file;
        // Parse optional -n flag
        size_t argIdx = 0;
        if (cmd.arguments.size() >= 2 && cmd.arguments[0] == "-n") {
            try { n = std::stoi(cmd.arguments[1]); } catch(...) {}
            argIdx = 2;
        }
        if (argIdx >= cmd.arguments.size()) { error("head", "missing file operand"); return; }
        file = resolvePath(cmd.arguments[argIdx]);

        size_t size = 0;
        unsigned char* data = fileio_read(file.c_str(), &size);
        if (!data) { error("head", ("cannot read '" + cmd.arguments[argIdx] + "'").c_str()); return; }

        std::string content(reinterpret_cast<char*>(data), size);
        free(data);

        int lineCount = 0;
        size_t pos = 0;
        while (pos < content.size() && lineCount < n) {
            size_t nl = content.find('\n', pos);
            std::string line = content.substr(pos, nl == std::string::npos ? nl : nl - pos);
            out(line);
            ++lineCount;
            if (nl == std::string::npos) break;
            pos = nl + 1;
        }
    };

    // ---- tail ----
    dispatch["tail"] = [this](const Parser::Command& cmd) {
        int n = 10;
        std::string file;
        size_t argIdx = 0;
        if (cmd.arguments.size() >= 2 && cmd.arguments[0] == "-n") {
            try { n = std::stoi(cmd.arguments[1]); } catch(...) {}
            argIdx = 2;
        }
        if (argIdx >= cmd.arguments.size()) { error("tail", "missing file operand"); return; }
        file = resolvePath(cmd.arguments[argIdx]);

        size_t size = 0;
        unsigned char* data = fileio_read(file.c_str(), &size);
        if (!data) { error("tail", ("cannot read '" + cmd.arguments[argIdx] + "'").c_str()); return; }

        std::string content(reinterpret_cast<char*>(data), size);
        free(data);

        // Collect lines
        std::vector<std::string> lines;
        size_t pos = 0;
        while (pos <= content.size()) {
            size_t nl = content.find('\n', pos);
            lines.push_back(content.substr(pos, nl == std::string::npos ? nl : nl - pos));
            if (nl == std::string::npos) break;
            pos = nl + 1;
        }

        size_t start = lines.size() > (size_t)n ? lines.size() - n : 0;
        for (size_t i = start; i < lines.size(); ++i) out(lines[i]);
    };

    // ---- wc ----
    dispatch["wc"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("wc", "missing file operand"); return; }
        std::string path = resolvePath(cmd.arguments[0]);

        size_t size = 0;
        unsigned char* data = fileio_read(path.c_str(), &size);
        if (!data) { error("wc", ("cannot read '" + cmd.arguments[0] + "'").c_str()); return; }

        size_t lines = 0, words = 0, bytes = size;
        bool inWord = false;
        for (size_t i = 0; i < size; ++i) {
            char c = (char)data[i];
            if (c == '\n') ++lines;
            if (std::isspace((unsigned char)c)) { inWord = false; }
            else if (!inWord) { inWord = true; ++words; }
        }
        free(data);

        out(std::to_string(lines) + "  " + std::to_string(words) + "  "
            + std::to_string(bytes) + "  " + cmd.arguments[0]);
    };

    // ---- find ----
    dispatch["find"] = [this](const Parser::Command& cmd) {
        // Usage: find [directory] <pattern>
        std::string dir, pattern;
        if (cmd.arguments.size() >= 2) {
            dir     = resolvePath(cmd.arguments[0]);
            pattern = cmd.arguments[1];
        } else if (cmd.arguments.size() == 1) {
            dir     = prompt.getCurrentDir();
            pattern = cmd.arguments[0];
        } else {
            error("find", "usage: find [directory] <pattern>");
            return;
        }

        namespace fs = std::filesystem;
        std::error_code ec;
        for (auto& entry : fs::recursive_directory_iterator(dir, ec)) {
            std::string name = entry.path().filename().string();
            if (name.find(pattern) != std::string::npos) {
                out(entry.path().string());
            }
        }
        if (ec) error("find", ("cannot access '" + dir + "'").c_str());
    };

    // ---- tree ----
    dispatch["tree"] = [this](const Parser::Command& cmd) {
        std::string dir = cmd.arguments.empty()
                          ? prompt.getCurrentDir()
                          : resolvePath(cmd.arguments[0]);

        namespace fs = std::filesystem;
        std::function<void(const fs::path&, const std::string&)> printTree;
        printTree = [&](const fs::path& p, const std::string& prefix) {
            std::error_code ec;
            std::vector<fs::directory_entry> entries;
            for (auto& e : fs::directory_iterator(p, ec)) entries.push_back(e);
            std::sort(entries.begin(), entries.end(),
                [](const fs::directory_entry& a, const fs::directory_entry& b) {
                    return a.path().filename() < b.path().filename();
                });

            for (size_t i = 0; i < entries.size(); ++i) {
                bool last = (i == entries.size() - 1);
                std::string connector = last ? "\xe2\x94\x94\xe2\x94\x80\xe2\x94\x80 "
                                             : "\xe2\x94\x9c\xe2\x94\x80\xe2\x94\x80 ";
                std::string name = entries[i].path().filename().string();
                bool isDir = entries[i].is_directory(ec);
                if (isDir)
                    out(prefix + connector + "\x1b[36m" + name + "/\x1b[0m");
                else
                    out(prefix + connector + name);
                if (isDir) {
                    std::string child_prefix = prefix + (last ? "    " : "\xe2\x94\x82   ");
                    printTree(entries[i].path(), child_prefix);
                }
            }
        };

        out("\x1b[36m" + dir + "\x1b[0m");
        printTree(fs::path(dir), "");
    };

    // ---- env ----
    dispatch["env"] = [](const Parser::Command&) {
        // Print our virtual environment (stored in platform env for now)
        extern char** environ;
        for (char** ep = environ; ep && *ep; ++ep)
            out(*ep);
    };

    // ---- export ----
    dispatch["export"] = [](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("export", "usage: export NAME=value"); return; }
        const std::string& arg = cmd.arguments[0];
        auto eq = arg.find('=');
        if (eq == std::string::npos) { error("export", "usage: export NAME=value"); return; }
        std::string name  = arg.substr(0, eq);
        std::string value = arg.substr(eq + 1);
        setenv(name.c_str(), value.c_str(), 1);
    };

    // ---- set ----
    dispatch["set"] = [](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) {
            extern char** environ;
            for (char** ep = environ; ep && *ep; ++ep) out(*ep);
            return;
        }
        const std::string& arg = cmd.arguments[0];
        auto eq = arg.find('=');
        if (eq == std::string::npos) { error("set", "usage: set NAME=value"); return; }
        std::string name  = arg.substr(0, eq);
        std::string value = arg.substr(eq + 1);
        setenv(name.c_str(), value.c_str(), 1);
    };

    // ---- unset ----
    dispatch["unset"] = [](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("unset", "missing variable name"); return; }
        unsetenv(cmd.arguments[0].c_str());
    };

    // ---- which ----
    dispatch["which"] = [](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("which", "missing command name"); return; }
        out("\x1b[36m(built-in)\x1b[0m  " + cmd.arguments[0]);
    };

    // ---- whereis ----
    dispatch["whereis"] = [](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("whereis", "missing command name"); return; }
        out(cmd.arguments[0] + ": \x1b[36m(built-in TinyShell command)\x1b[0m");
    };

    // ---- history ----
    dispatch["history"] = [this](const Parser::Command&) {
        const auto& entries = history.entries();
        if (entries.empty()) { out("(no history)"); return; }
        for (size_t i = 0; i < entries.size(); ++i) {
            out("  " + std::to_string(i + 1) + "  " + entries[i]);
        }
    };

    // ---- run ----
    dispatch["run"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("run", "missing executable path"); return; }
        process_create(cmd.arguments[0].c_str());
    };

    // ---- setram ----
    dispatch["setram"] = [this](const Parser::Command& cmd) {
        if (cmd.arguments.empty()) { error("setram", "missing argument"); return; }
        std::string arg = cmd.arguments[0];
        if (arg == "auto") {
            fileio_write("tinyvm.conf", "auto", 4);
            out("Automatic RAM allocation restored.");
            out("Restart TinyVM for the new memory size to take effect.");
            return;
        }
        char unit = arg.back();
        uint64_t multiplier = 1;
        if      (unit == 'k' || unit == 'K') multiplier = 1024ULL;
        else if (unit == 'm' || unit == 'M') multiplier = 1024ULL * 1024ULL;
        else if (unit == 'g' || unit == 'G') multiplier = 1024ULL * 1024ULL * 1024ULL;

        std::string numPart = arg;
        if (multiplier != 1) numPart.pop_back();
        try {
            uint64_t val   = std::stoull(numPart);
            uint64_t bytes = val * multiplier;
            uint64_t host  = ramauto_host_ram();
            if (bytes == 0) {
                error("setram", "RAM size must be greater than zero");
            } else if (bytes < TVM_MIN_RAM) {
                error("setram", ("minimum supported RAM is "
                      + std::string(ramauto_to_string(TVM_MIN_RAM))).c_str());
            } else if (bytes > host) {
                error("setram", ("cannot exceed host RAM ("
                      + std::string(ramauto_to_string(host)) + ")").c_str());
            } else {
                std::string s = std::to_string(bytes);
                fileio_write("tinyvm.conf", s.c_str(), s.size());
                out("Virtual RAM configured to " + std::string(ramauto_to_string(bytes)) + ".");
                out("Restart TinyVM for the new memory size to take effect.");
            }
        } catch (...) {
            error("setram", "malformed value");
        }
    };

    // ---- ram ----
    dispatch["ram"] = [](const Parser::Command&) {
        uint64_t host = ramauto_host_ram();
        uint64_t configured = TVM_RAM_AUTO;
        bool is_auto = true;
        size_t conf_size = 0;
        unsigned char* conf_data = fileio_read("tinyvm.conf", &conf_size);
        if (conf_data) {
            std::string s((char*)conf_data, conf_size);
            if (s.substr(0, 4) != "auto") {
                try { configured = std::stoull(s); is_auto = false; } catch(...) {}
            }
            free(conf_data);
        }
        uint64_t resolved = ramauto_resolve(configured);

        out("\x1b[33mHost RAM\x1b[0m        : " + std::string(ramauto_to_string(host)));
        out("\x1b[33mCurrent VRAM\x1b[0m    : " + std::string(ramauto_to_string(current_virtual_ram)));
        out("\x1b[33mConfigured VRAM\x1b[0m : " + std::string(ramauto_to_string(resolved)));
        out(std::string("\x1b[33mMode\x1b[0m            : ") + (is_auto ? "Automatic (1/64 host)" : "Manual"));
        bool active = current_virtual_ram == resolved;
        out(std::string("\x1b[33mStatus\x1b[0m          : ") + (active ? "\x1b[32mActive\x1b[0m" : "\x1b[33mPending Restart\x1b[0m"));
    };
}

// ============================================================
// execute()
// ============================================================

void Executor::execute(const Parser::Command& cmd)
{
    if (cmd.program.empty()) return;

    auto it = dispatch.find(cmd.program);
    if (it != dispatch.end()) {
        it->second(cmd);
    } else {
        error(cmd.program.c_str(),
              "command not found \xe2\x80\x94 type 'help' for a list of commands");
    }
}

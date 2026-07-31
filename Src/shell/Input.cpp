#include "Input.h"
#include "Completion.h"
#include "../console/console.h"
#include "../TinyKernel/kernel.h"
#include <cstdlib>
#include <string>

// ============================================================
// Input::update()
// Polls the console for:
//   - Completed lines (Enter key)
//   - Up/Down arrow  → history navigation
//   - Tab            → tab completion
// ============================================================

void Input::update(History& history, Completion& completion,
                   const std::string& cwd)
{
    if (lineReady) return;

    Console* cpp_console = reinterpret_cast<Console*>(console);

    // ---- History: Up arrow (older) ----
    if (cpp_console->consumeHistoryUp()) {
        if (!recalling) {
            // Save the live input before starting recall
            savedInput = cpp_console->inputLine();
            recalling  = true;
        }
        std::string entry = history.navigate(-1);
        cpp_console->setInputLine(entry);
        return;
    }

    // ---- History: Down arrow (newer) ----
    if (cpp_console->consumeHistoryDown()) {
        std::string entry = history.navigate(+1);
        if (entry.empty() && recalling) {
            // Returned to live position
            cpp_console->setInputLine(savedInput);
            recalling = false;
        } else {
            cpp_console->setInputLine(entry);
        }
        return;
    }

    // ---- Tab completion ----
    if (cpp_console->consumeTab()) {
        std::string current = cpp_console->inputLine();
        // Find the word being completed (last whitespace-separated token)
        std::size_t wordStart = current.rfind(' ');
        std::string prefix;
        std::string before;
        if (wordStart == std::string::npos) {
            prefix = current;
            before = "";
        } else {
            before = current.substr(0, wordStart + 1);
            prefix = current.substr(wordStart + 1);
        }

        auto matches = completion.complete(prefix, cwd);

        if (matches.size() == 1) {
            // Single match: auto-complete
            cpp_console->setInputLine(before + matches[0]);
        } else if (matches.size() > 1) {
            // Multiple matches: show them on a new line
            // Find the longest common prefix
            std::string common = matches[0];
            for (const auto& m : matches) {
                std::size_t i = 0;
                while (i < common.size() && i < m.size() && common[i] == m[i]) ++i;
                common = common.substr(0, i);
            }
            // Extend to longest common prefix
            if (common.size() > prefix.size()) {
                cpp_console->setInputLine(before + common);
            } else {
                // Print completions to console (like bash does)
                std::string line;
                for (const auto& m : matches) {
                    line += m;
                    line += "  ";
                }
                // Write the current input, a newline, the matches, then the prompt continues
                console_write_line(console, "");
                console_write_line(console, line.c_str());
            }
        }
        return;
    }

    // ---- Normal line read ----
    char* line = console_read_line(console);
    if (line) {
        if (line[0] != '\0') {
            bufferedLine = line;
            lineReady    = true;
            recalling    = false; // reset recall state on Enter
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

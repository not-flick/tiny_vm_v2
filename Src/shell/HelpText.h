#pragma once
// ============================================================
// HelpText  –  Grouped command help for TinyShell
// ============================================================
// Provides two functions:
//   printHelp()        – prints all commands grouped by category
//   printHelp(cmd)     – prints description, usage, and examples
//                        for a specific command
// ============================================================

#include <string>

// Forward-declare ConsoleHandle so HelpText.h doesn't have to
// pull in the full console.h in files that only need help text.
typedef struct Console ConsoleHandle;

namespace HelpText {

// Print the grouped command listing to the console.
void printAll(ConsoleHandle* console);

// Print per-command help.  Returns false if the command is unknown.
bool printCommand(ConsoleHandle* console, const std::string& cmd);

} // namespace HelpText

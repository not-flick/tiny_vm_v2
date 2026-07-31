#pragma once
#include <string>
#include "History.h"

// ============================================================
// Input  –  Reads completed lines from the Console
// ============================================================
// Also handles:
//   - Up/Down arrow history navigation (via Console flags)
//   - Tab completion (via Console flags)
// The shell's History and Completion objects are passed in so
// Input can drive them without coupling Console to the shell.
// ============================================================

class Completion; // forward decl

class Input {
public:
    void update(History& history, Completion& completion,
                const std::string& cwd);

    bool        hasLine()  const;
    std::string getLine();

private:
    std::string bufferedLine;
    bool        lineReady  = false;

    // Saved live input before history recall starts.
    std::string savedInput;
    bool        recalling  = false;
};

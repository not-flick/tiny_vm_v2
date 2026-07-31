#pragma once
#include "Input.h"
#include "Parser.h"
#include "Executor.h"
#include "History.h"
#include "ShellPrompt.h"
#include "Renderer.h"
#include "Completion.h"

class ShellSystem {
public:
    ShellSystem();
    void update();
private:
    History     history;
    ShellPrompt prompt;
    Completion  completion;
    Input       input;
    Parser      parser;
    Executor    executor;
    Renderer    renderer;
    bool        needsPrompt = true;
};

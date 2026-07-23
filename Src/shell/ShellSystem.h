#pragma once
#include "Input.h"
#include "Parser.h"
#include "Executor.h"
#include "History.h"
#include "ShellPrompt.h"
#include "Renderer.h"

class ShellSystem {
public:
    ShellSystem();
    void update();
private:
    Input input;
    Parser parser;
    Executor executor;
    History history;
    ShellPrompt prompt;
    Renderer renderer;
    bool needsPrompt = true;
};

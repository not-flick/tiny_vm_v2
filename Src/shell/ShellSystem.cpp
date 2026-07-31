#include "ShellSystem.h"

ShellSystem::ShellSystem()
    : executor(prompt, history)
{
    // executor is constructed with references to prompt and history
    // so commands like 'cd' and 'history' have access to shell state.
}

void ShellSystem::update()
{
    if (needsPrompt) {
        prompt.display();
        needsPrompt = false;
    }

    // Input::update() handles:
    //   - Normal line input (Enter)
    //   - Up/Down arrow history navigation
    //   - Tab completion
    input.update(history, completion, prompt.getCurrentDir());

    if (input.hasLine()) {
        std::string line = input.getLine();
        history.add(line);
        history.resetCursor(); // always reset cursor after submitting

        Parser::Command cmd = parser.parse(line);
        executor.execute(cmd);

        needsPrompt = true;
    }

    renderer.render();
}

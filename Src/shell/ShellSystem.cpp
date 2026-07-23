#include "ShellSystem.h"

ShellSystem::ShellSystem() {
}

void ShellSystem::update() {
    if (needsPrompt) {
        prompt.display();
        needsPrompt = false;
    }
    
    input.update();
    
    if (input.hasLine()) {
        std::string line = input.getLine();
        history.add(line);
        
        Parser::Command cmd = parser.parse(line);
        executor.execute(cmd);
        
        needsPrompt = true;
    }
    
    renderer.render();
}

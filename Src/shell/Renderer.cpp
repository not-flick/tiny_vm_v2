#include "Renderer.h"

void Renderer::render() {
    // In this architecture, the Console module already handles rendering the text buffer.
    // The Shell Renderer module can be responsible for ensuring the prompt is drawn or 
    // integrating with the console's rendering if needed.
    // For now, Console::render() is handled by the main loop.
}

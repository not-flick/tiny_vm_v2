#include "console.h"

#include "../Platform/platform.h"

#include <filesystem>

bool Console::loadFont(const std::filesystem::path& path, float size)
{
    unloadFont();

    font = TTF_OpenFont(path.string().c_str(), size);

    if (font == nullptr)
    {
        SDL_Log("Failed to load font: %s", path.string().c_str());
        return false;
    }

    currentFont = path;
    currentFontSize = size;

    return true;
}

bool Console::loadDefaultFont(float size)
{
    currentFontSize = size;

    // Use the strictly monospaced bundled terminal font.
    // This provides a consistent system terminal look across all platforms.
    return loadFont(
        Platform::fontsDirectory() /
        "JetBrainsMonoNerdFontMono-Regular.ttf",
        size
    );
}

void Console::unloadFont()
{
    if (font)
    {
        TTF_CloseFont(font);
        font = nullptr;
    }

    currentFont.clear();
}

bool Console::hasFont() const
{
    return font != nullptr;
}

float Console::fontSize() const
{
    return currentFontSize;
}
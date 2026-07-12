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

    // Try every system font first.
    for (const auto& directory : Platform::systemFontDirectories())
    {
        if (!std::filesystem::exists(directory))
            continue;

        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(directory))
        {
            if (!entry.is_regular_file())
                continue;

            auto extension = entry.path().extension().string();

            if (extension != ".ttf" &&
                extension != ".otf" &&
                extension != ".ttc")
            {
                continue;
            }

            if (loadFont(entry.path(), size))
                return true;
        }
    }

    // Fall back to the bundled Nerd Font.
    return loadFont(
        Platform::fontsDirectory() /
        "JetBrainsMonoNerdFontPropo-Regular.ttf",
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
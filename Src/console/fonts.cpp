#include "console.h"
#include "platform.h"

#include <filesystem>

bool Console::loadFont(const std::filesystem::path& path, float size)
{
    unloadFont();

    font = TTF_OpenFont(path.string().c_str(), size);

    if (!font)
        return false;

    currentFont = path;
    currentFontSize = size;

    return true;
}

bool Console::loadDefaultFont(float size)
{
    // Try the bundled JetBrains Nerd Font.
    if (loadFont(
        Platform::fontsDirectory() /
        "JetBrainsMonoNerdFontPropo-Regular.ttf",
        size))
    {
        return true;
    }

    // Try every system font.
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

    return false;
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
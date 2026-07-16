#include "platform.h"

#include <cstdlib>

OperatingSystem Platform::current()
{
#ifdef _WIN32
    return OperatingSystem::Windows;
#elif defined(__linux__)
    return OperatingSystem::Linux;
#elif defined(__APPLE__)
    return OperatingSystem::MacOS;
#else
    return OperatingSystem::Unknown;
#endif
}

std::string Platform::name()
{
    switch (current())
    {
        case OperatingSystem::Windows: return "Windows";
        case OperatingSystem::Linux:   return "Linux";
        case OperatingSystem::MacOS:   return "macOS";
        default:                       return "Unknown";
    }
}

std::filesystem::path Platform::homeDirectory()
{
#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif

    if (home)
        return home;

    return {};
}

std::filesystem::path Platform::executableDirectory()
{
    return std::filesystem::current_path();
}

std::filesystem::path Platform::rootDirectory()
{
    auto current = executableDirectory();
    
    // Check if Assets is in the current directory
    if (std::filesystem::exists(current / "Assets"))
        return current;

    // Check if we are running from a build subdirectory (like 'build' or 'Build')
    if (std::filesystem::exists(current.parent_path() / "Assets"))
        return current.parent_path();

    // Default fallback
    return current;
}

std::filesystem::path Platform::assetsDirectory()
{
    return rootDirectory() / "Assets";
}

std::filesystem::path Platform::fontsDirectory()
{
    return assetsDirectory() / "fonts";
}

std::vector<std::filesystem::path> Platform::systemFontDirectories()
{
    switch (current())
    {
        case OperatingSystem::Windows:
            return {
                "C:/Windows/Fonts",
                homeDirectory() / "AppData/Local/Microsoft/Windows/Fonts"
            };

        case OperatingSystem::Linux:
            return {
                "/usr/share/fonts",
                "/usr/local/share/fonts",
                homeDirectory() / ".fonts",
                homeDirectory() / ".local/share/fonts"
            };

        case OperatingSystem::MacOS:
            return {
                "/System/Library/Fonts",
                "/Library/Fonts",
                homeDirectory() / "Library/Fonts"
            };

        default:
            return {};
    }
}

std::vector<std::filesystem::path> Platform::systemFonts()
{
    std::vector<std::filesystem::path> fonts;

    for (const auto& directory : systemFontDirectories())
        scanFonts(directory, fonts);

    return fonts;
}

void Platform::scanFonts(
    const std::filesystem::path&,
    std::vector<std::filesystem::path>&)
{
    // TODO
}
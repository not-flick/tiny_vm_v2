#pragma once

#include <string>
#include <vector>
#include <filesystem>

enum class OperatingSystem
{
    Windows,
    Linux,
    MacOS,
    Unknown
};

class Platform
{
public:

    static OperatingSystem current();

    static std::string name();

    static std::filesystem::path homeDirectory();

    static std::filesystem::path executableDirectory();

    static std::filesystem::path rootDirectory();

    static std::filesystem::path assetsDirectory();

    static std::filesystem::path fontsDirectory();

    static std::vector<std::filesystem::path> systemFontDirectories();

    static std::vector<std::filesystem::path> systemFonts();

private:

    static void scanFonts(
        const std::filesystem::path& directory,
        std::vector<std::filesystem::path>& fonts
    );
};
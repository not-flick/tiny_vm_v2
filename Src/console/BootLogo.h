#pragma once

#include <filesystem>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

class BootLogo {
public:
    BootLogo() = default;
    ~BootLogo();

    // Loads the PNG texture. Returns false if the file is missing or fails to load.
    bool load(SDL_Renderer* renderer, const std::filesystem::path& path);

    // Shows the splash screen for the specified duration (in milliseconds).
    // Safely handles non-blocking event polling internally.
    void show(SDL_Renderer* renderer, SDL_Window* window, int duration_ms = 2000);

    // Cleans up the texture early if needed.
    void unload();

private:
    SDL_Texture* m_texture = nullptr;
    int m_width = 0;
    int m_height = 0;
};

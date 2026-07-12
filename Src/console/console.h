#pragma once

#include <string>
#include <string_view>
#include <filesystem>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Console
{
public:

    // Creates the console window.
    Console(
        int width = 1280,
        int height = 720,
        std::string_view title = "TinyVM"
    );

    // Cleans up SDL resources.
    ~Console();


    // =============================
    // Output
    // =============================

    void write(std::string_view text);
    void writeLine(std::string_view text);


    // =============================
    // Input
    // =============================

    std::string readLine();


    // =============================
    // Screen
    // =============================

    void clear();


    // =============================
    // Window
    // =============================

    void resize(int width, int height);

    void setTitle(std::string_view title);


    // =============================
    // Main Loop
    // =============================

    bool isOpen();

    void pollEvents();

    void present();

    // =============================
    // Fonts
    // =============================

    bool loadFont(const std::filesystem::path& path, float size);

    bool loadDefaultFont(float size = 18.0f);

    void unloadFont();

    bool hasFont() const;

    float fontSize() const;


private:

    // SDL objects
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    // Window state
    bool running = false;

    // Window size
    int windowWidth = 0;
    int windowHeight = 0;

    std::filesystem::path currentFont;
    float currentFontSize = 18.0f;

    void draw(
        std::string_view text,
        int x,
        int y
    );

    void drawCharacter(
        char32_t character,
        int x,
        int y
    );
    
    void render();
};
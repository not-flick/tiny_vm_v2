#include "console.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

Console::Console(int width, int height, std::string_view title)
{
    windowWidth  = width;
    windowHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        running = false;
        return;
    }

    if (!TTF_Init())
    {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());

        SDL_Quit();

        running = false;
        return;
    }

    window = SDL_CreateWindow(
        title.data(),
        width,
        height,
        SDL_WINDOW_RESIZABLE
    );

    if (!window)
    {
        SDL_Log("Window creation failed: %s", SDL_GetError());

        TTF_Quit();
        SDL_Quit();

        running = false;
        return;
    }

    renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer)
    {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());

        SDL_DestroyWindow(window);

        TTF_Quit();
        SDL_Quit();

        running = false;
        return;
    }

    running = true;

    SDL_StartTextInput(window);

    if (!loadDefaultFont(16.0f))
    {
        SDL_Log("Failed to load default font.");
    }
}

Console::~Console()
{
    unloadBanner();

    if (font)
        TTF_CloseFont(font);

    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

bool Console::isOpen()
{
    return running;
}

void Console::pollEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                windowWidth  = event.window.data1;
                windowHeight = event.window.data2;
                break;

            case SDL_EVENT_TEXT_INPUT:
                currentInput += event.text.text;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_BACKSPACE) {
                    if (!currentInput.empty()) {
                        // Assuming ASCII for now. Proper UTF-8 backspace requires more complex handling.
                        currentInput.pop_back();
                    }
                } else if (event.key.key == SDLK_RETURN || event.key.key == SDLK_KP_ENTER) {
                    writeLine(currentInput);
                    enterPressed = true;
                }
                break;
        }
    }
}


void Console::present()
{
    render();
}

void Console::write(std::string_view text) {
    if (textLines.empty()) {
        textLines.push_back({});
    }
    
    size_t i = 0;
    std::string currentText = "";
    
    auto commitText = [&]() {
        if (!currentText.empty()) {
            textLines.back().push_back({currentText, currentColorR, currentColorG, currentColorB});
            currentText.clear();
        }
    };

    while (i < text.length()) {
        if (text[i] == '\n') {
            commitText();
            textLines.push_back({});
            i++;
        } else if (text[i] == '\x1b' && i + 1 < text.length() && text[i+1] == '[') {
            commitText();
            size_t end = text.find('m', i + 2);
            if (end != std::string_view::npos) {
                std::string_view code = text.substr(i + 2, end - (i + 2));
                if (code == "32") { // Green
                    currentColorR = 0; currentColorG = 255; currentColorB = 0;
                } else if (code == "31") { // Red
                    currentColorR = 255; currentColorG = 0; currentColorB = 0;
                } else if (code == "33") { // Yellow
                    currentColorR = 255; currentColorG = 255; currentColorB = 0;
                } else if (code == "34") { // Blue
                    currentColorR = 0; currentColorG = 0; currentColorB = 255;
                } else if (code == "36") { // Cyan
                    currentColorR = 0; currentColorG = 255; currentColorB = 255;
                } else if (code == "37") { // White
                    currentColorR = 255; currentColorG = 255; currentColorB = 255;
                } else if (code == "0") { // Reset
                    currentColorR = 255; currentColorG = 255; currentColorB = 255;
                }
                i = end + 1;
            } else {
                i += 2; // Skip \x1b[ if malformed
            }
        } else {
            currentText += text[i];
            i++;
        }
    }
    commitText();
}

void Console::writeLine(std::string_view text) {
    write(text);
    write("\n");
}

std::string Console::readLine() {
    if (enterPressed) {
        std::string res = currentInput;
        currentInput.clear();
        enterPressed = false;
        return res;
    }
    return "";
}

void Console::clear() {
    textLines.clear();
}

void Console::resize(int width, int height) {
    SDL_SetWindowSize(window, width, height);
    windowWidth = width;
    windowHeight = height;
}

void Console::setTitle(std::string_view title) {
    SDL_SetWindowTitle(window, title.data());
}

void Console::close() {
    running = false;
}

bool Console::loadBanner(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) return false;
    bannerTexture = IMG_LoadTexture(renderer, path.string().c_str());
    if (!bannerTexture) {
        SDL_Log("Failed to load banner: %s", SDL_GetError());
        return false;
    }
    
    float w, h;
    if (SDL_GetTextureSize(bannerTexture, &w, &h)) {
        bannerWidth = static_cast<int>(w);
        bannerHeight = static_cast<int>(h);
        return true;
    }
    unloadBanner();
    return false;
}

void Console::unloadBanner() {
    if (bannerTexture) {
        SDL_DestroyTexture(bannerTexture);
        bannerTexture = nullptr;
    }
}
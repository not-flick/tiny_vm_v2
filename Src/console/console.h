#pragma once

#ifdef __cplusplus
#include <string>
#include <string_view>
#include <filesystem>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

struct TextSegment {
    std::string text;
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
};

using TextLine = std::vector<TextSegment>;

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
    float currentFontSize = 14.0f;
    std::vector<TextLine> textLines;
    
    uint8_t currentColorR = 255;
    uint8_t currentColorG = 255;
    uint8_t currentColorB = 255;

    int draw(
        std::string_view text,
        int x,
        int y,
        uint8_t r = 255,
        uint8_t g = 255,
        uint8_t b = 255
    );

    void drawCharacter(
        char32_t character,
        int x,
        int y
    );

    void render();


};
#endif // __cplusplus

// C API bindings
#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

typedef struct Console ConsoleHandle;

ConsoleHandle* console_create(int width, int height, const char* title);
void console_destroy(ConsoleHandle* console);

void console_write(ConsoleHandle* console, const char* text);
void console_write_line(ConsoleHandle* console, const char* text);
char* console_read_line(ConsoleHandle* console);

void console_clear(ConsoleHandle* console);

void console_resize(ConsoleHandle* console, int width, int height);
void console_set_title(ConsoleHandle* console, const char* title);

bool console_is_open(ConsoleHandle* console);
void console_poll_events(ConsoleHandle* console);
void console_present(ConsoleHandle* console);

bool console_load_font(ConsoleHandle* console, const char* path, float size);
bool console_load_default_font(ConsoleHandle* console, float size);
void console_unload_font(ConsoleHandle* console);
bool console_has_font(ConsoleHandle* console);
float console_font_size(ConsoleHandle* console);

#ifdef __cplusplus
}
#endif
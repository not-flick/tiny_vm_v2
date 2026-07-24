#pragma once

#ifdef __cplusplus
#include <string>
#include <string_view>
#include <filesystem>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// ---- scrollback.h already defines TextSegment / TextLine ----
#include "scrollback.h"
#include "viewport.h"
#include "selection.h"

// NOTE: TextSegment and TextLine are defined in scrollback.h.
//       They are reproduced here so that code which only includes
//       console.h also gets those types.
// (scrollback.h guards against double-definition with #pragma once)

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

    void close();

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

    SDL_Renderer* getRenderer() const { return renderer; }
    SDL_Window*   getWindow()   const { return window; }


    bool loadBanner(const std::filesystem::path& path);
    void unloadBanner();

private:

    // ---- SDL objects -----------------------------------------
    SDL_Window*   window         = nullptr;
    SDL_Renderer* renderer       = nullptr;
    TTF_Font*     font           = nullptr;
    SDL_Texture*  bannerTexture  = nullptr;

    // ---- Window state ----------------------------------------
    bool running      = false;
    int  windowWidth  = 0;
    int  windowHeight = 0;
    int  bannerWidth  = 0;
    int  bannerHeight = 0;

    // ---- Font ------------------------------------------------
    std::filesystem::path currentFont;
    float                 currentFontSize = 14.0f;

    // ---- Scrollback / Viewport / Selection -------------------
    ScrollbackBuffer scrollback;
    Viewport         viewport;
    Selection        selection;

    // ---- Current write state ---------------------------------
    // The "pending" line accumulates segments until a newline
    // is written; then it is committed to the scrollback buffer.
    TextLine currentLine;          // segments being assembled

    uint8_t currentColorR = 255;
    uint8_t currentColorG = 255;
    uint8_t currentColorB = 255;

    // ---- Input -----------------------------------------------
    std::string currentInput;
    bool        enterPressed = false;

    // ---- Mouse state (for selection) -------------------------
    bool        mouseDown        = false;
    Uint32      lastClickTime    = 0;
    int         clickCount       = 0;
    int         lastClickX       = 0;
    int         lastClickY       = 0;

    // ---- Private helpers -------------------------------------
    int  lineHeight() const;
    int  bannerBottom() const;

    // Convert pixel (y) inside text area → scrollback line index.
    // Returns SIZE_MAX if outside the text area.
    std::size_t pixelToLineIndex(int y) const;

    // Convert pixel (x) on a given rendered line → byte column.
    std::size_t pixelToColumn(int x, std::size_t lineIdx) const;

    // Build the plain-text of a single scrollback line.
    std::string lineText(std::size_t idx) const;

    // Copy selected text to clipboard.
    void copySelection();

    // Draw a single string; returns the pixel width rendered.
    int draw(
        std::string_view text,
        int x, int y,
        uint8_t r = 255, uint8_t g = 255, uint8_t b = 255
    );

    // Draw highlighted (selected) background rectangle.
    void drawHighlight(int x, int y, int w, int h);

    void drawCharacter(char32_t character, int x, int y);

    void render();
};
#endif // __cplusplus

// ============================================================
// C API bindings
// ============================================================
#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

typedef struct Console ConsoleHandle;

ConsoleHandle* console_create(int width, int height, const char* title);
void           console_destroy(ConsoleHandle* console);

void  console_write(ConsoleHandle* console, const char* text);
void  console_write_line(ConsoleHandle* console, const char* text);
char* console_read_line(ConsoleHandle* console);

void console_clear(ConsoleHandle* console);

void console_resize(ConsoleHandle* console, int width, int height);
void console_set_title(ConsoleHandle* console, const char* title);
void console_close(ConsoleHandle* console);

bool console_is_open(ConsoleHandle* console);
void console_poll_events(ConsoleHandle* console);
void console_present(ConsoleHandle* console);

bool  console_load_font(ConsoleHandle* console, const char* path, float size);
bool  console_load_default_font(ConsoleHandle* console, float size);
void  console_unload_font(ConsoleHandle* console);
bool  console_has_font(ConsoleHandle* console);
float console_font_size(ConsoleHandle* console);

#ifdef __cplusplus
}
#endif
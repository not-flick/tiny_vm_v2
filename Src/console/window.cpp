// ============================================================
// window.cpp  –  Console window, event handling, text I/O
// ============================================================
#include "console.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

// ============================================================
// Construction / destruction
// ============================================================

Console::Console(int width, int height, std::string_view title)
{
    windowWidth  = width;
    windowHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        SDL_Quit();
        return;
    }

    window = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }

    // Enable blend mode so selection highlights can be drawn semi-transparently.
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    running = true;

    SDL_StartTextInput(window);

    if (!loadDefaultFont(16.0f))
        SDL_Log("Failed to load default font.");

    // Seed the scrollback with an empty first line so write() always has
    // somewhere to append segments.
    scrollback.pushLine({});
}

Console::~Console()
{
    if (font)     TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window)   SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

// ============================================================
// Basic state
// ============================================================

bool Console::isOpen() { return running; }

void Console::close()  { running = false; }

void Console::resize(int width, int height) {
    SDL_SetWindowSize(window, width, height);
    windowWidth  = width;
    windowHeight = height;
}

void Console::setTitle(std::string_view title) {
    SDL_SetWindowTitle(window, title.data());
}

// ============================================================
// Event handling
// ============================================================

void Console::pollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        // ---- quit ------------------------------------------------
        case SDL_EVENT_QUIT:
            running = false;
            break;

        // ---- window resize ---------------------------------------
        case SDL_EVENT_WINDOW_RESIZED:
            windowWidth  = event.window.data1;
            windowHeight = event.window.data2;
            break;

        // ---- keyboard text input ---------------------------------
        case SDL_EVENT_TEXT_INPUT:
            currentInput += event.text.text;
            break;

        // ---- keyboard special keys ------------------------------
        case SDL_EVENT_KEY_DOWN:
        {
            SDL_Keymod mod = SDL_GetModState();
            bool ctrl  = (mod & SDL_KMOD_CTRL)  != 0;
            bool shift = (mod & SDL_KMOD_SHIFT) != 0;

            switch (event.key.key)
            {
            // -- text editing --
            case SDLK_BACKSPACE:
                if (!currentInput.empty())
                    currentInput.pop_back();
                break;

            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                writeLine(currentInput);
                enterPressed = true;
                break;

            // -- history navigation --
            case SDLK_UP:
                historyUpPending = true;
                break;

            case SDLK_DOWN:
                historyDownPending = true;
                break;

            // -- tab completion --
            case SDLK_TAB:
                tabPending = true;
                break;

            // -- clipboard --
            case SDLK_C:
                if (ctrl && shift) {
                    // Ctrl+Shift+C → copy selection always
                    copySelection();
                } else if (ctrl && !shift) {
                    // Ctrl+C → copy only when something is selected;
                    // otherwise fall through (shell receives interrupt).
                    if (selection.active) {
                        copySelection();
                    }
                    // If no selection, do NOT consume the event –
                    // the shell's interrupt handling sees it naturally
                    // because we simply do nothing here.
                }
                break;

            case SDLK_V:
                if (ctrl && shift) {
                    // Ctrl+Shift+V → paste from clipboard
                    const char* cb = SDL_GetClipboardText();
                    if (cb && *cb) {
                        currentInput += cb;
                    }
                }
                break;

            // -- scrolling --
            case SDLK_PAGEUP:
            {
                int lh = lineHeight();
                std::size_t visLines = (lh > 0) ? windowHeight / lh : 1;
                viewport.pageUp(scrollback.size(), visLines);
                break;
            }
            case SDLK_PAGEDOWN:
            {
                int lh = lineHeight();
                std::size_t visLines = (lh > 0) ? windowHeight / lh : 1;
                viewport.pageDown(visLines);
                break;
            }
            case SDLK_HOME:
            {
                viewport.goHome(scrollback.size());
                break;
            }
            case SDLK_END:
                viewport.goEnd();
                break;

            // -- select all (Ctrl+A) --
            case SDLK_A:
                if (ctrl) {
                    if (scrollback.size() > 0) {
                        selection.anchor = {0, 0};
                        std::size_t last = scrollback.size() - 1;
                        selection.cursor = {last, lineText(last).size()};
                        selection.active = true;
                    }
                }
                break;

            default:
                break;
            }
            break; // SDL_EVENT_KEY_DOWN
        }

        case SDL_EVENT_MOUSE_WHEEL:
        {
            // event.wheel.y > 0 → scroll up (toward older output)
            if (event.wheel.y > 0) {
                viewport.scrollUp(3, scrollback.size());
            } else if (event.wheel.y < 0) {
                viewport.scrollDown(3);
            }
            break;
        }

        // ---- mouse button (selection) ---------------------------
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                int mx = (int)event.button.x;
                int my = (int)event.button.y;
                mouseDown = true;

                // Detect double / triple click
                Uint32 now = SDL_GetTicks();
                if (now - lastClickTime < 400 &&
                    std::abs(mx - lastClickX) < 8 &&
                    std::abs(my - lastClickY) < 8) {
                    ++clickCount;
                } else {
                    clickCount = 1;
                }
                lastClickTime = now;
                lastClickX    = mx;
                lastClickY    = my;

                std::size_t lineIdx = pixelToLineIndex(my);
                std::size_t col     = (lineIdx != SIZE_MAX)
                                      ? pixelToColumn(mx, lineIdx)
                                      : 0;

                if (clickCount == 3 && lineIdx != SIZE_MAX) {
                    // Triple-click: select whole line
                    selection.selectLine(lineIdx, lineText(lineIdx));
                } else if (clickCount == 2 && lineIdx != SIZE_MAX) {
                    // Double-click: select word
                    selection.selectWord(lineIdx, col, lineText(lineIdx));
                } else {
                    // Single click: begin drag-selection
                    if (lineIdx != SIZE_MAX)
                        selection.begin({lineIdx, col});
                    else
                        selection.clear();
                }
            }
            break;

        case SDL_EVENT_MOUSE_MOTION:
            if (mouseDown && selection.dragging) {
                int mx = (int)event.motion.x;
                int my = (int)event.motion.y;
                std::size_t lineIdx = pixelToLineIndex(my);
                if (lineIdx != SIZE_MAX) {
                    std::size_t col = pixelToColumn(mx, lineIdx);
                    selection.update({lineIdx, col});
                }
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                mouseDown = false;
                if (selection.dragging) {
                    int mx = (int)event.button.x;
                    int my = (int)event.button.y;
                    std::size_t lineIdx = pixelToLineIndex(my);
                    std::size_t col     = (lineIdx != SIZE_MAX)
                                          ? pixelToColumn(mx, lineIdx)
                                          : 0;
                    if (lineIdx != SIZE_MAX)
                        selection.finish({lineIdx, col});
                    else
                        selection.dragging = false;
                }
            }
            break;

        default:
            break;
        }
    }
}

void Console::present()
{
    render();
}

// ============================================================
// Text output
// ============================================================

void Console::write(std::string_view text)
{
    size_t i = 0;

    auto commitSegment = [&](const std::string& seg) {
        if (seg.empty()) return;
        // Append to the current (last) line in the scrollback buffer.
        scrollback.lastLine().push_back(
            {seg, currentColorR, currentColorG, currentColorB});
    };

    std::string pending;

    while (i < text.length()) {
        if (text[i] == '\n') {
            commitSegment(pending);
            pending.clear();
            // Finalize this line; start a fresh empty line.
            scrollback.pushLine({});

            // Auto-follow: keep viewport pinned if it was already at bottom.
            viewport.onNewLine();


            ++i;
        }
        else if (text[i] == '\x1b' && i + 1 < text.length() && text[i+1] == '[')
        {
            // ANSI escape sequence
            commitSegment(pending);
            pending.clear();

            size_t end = text.find('m', i + 2);
            if (end != std::string_view::npos) {
                std::string_view code = text.substr(i + 2, end - (i + 2));
                if      (code == "32") { currentColorR=0;   currentColorG=200; currentColorB=0;   }
                else if (code == "31") { currentColorR=220; currentColorG=0;   currentColorB=0;   }
                else if (code == "33") { currentColorR=220; currentColorG=220; currentColorB=0;   }
                else if (code == "34") { currentColorR=80;  currentColorG=130; currentColorB=255; }
                else if (code == "35") { currentColorR=180; currentColorG=0;   currentColorB=220; }
                else if (code == "36") { currentColorR=0;   currentColorG=210; currentColorB=220; }
                else if (code == "37") { currentColorR=255; currentColorG=255; currentColorB=255; }
                else if (code == "0")  { currentColorR=255; currentColorG=255; currentColorB=255; }
                i = end + 1;
            } else {
                i += 2; // malformed – skip
            }
        }
        else {
            pending += text[i];
            ++i;
        }
    }
    commitSegment(pending);
}

void Console::writeLine(std::string_view text) {
    write(text);
    write("\n");
}

bool Console::writeImage(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) return false;
    SDL_Texture* tex = IMG_LoadTexture(renderer, path.string().c_str());
    if (!tex) return false;

    float w, h;
    if (SDL_GetTextureSize(tex, &w, &h)) {
        ConsoleEntry entry;
        entry.type = ConsoleEntry::Type::Image;
        entry.texture = tex;
        entry.imageWidth = static_cast<int>(w);
        entry.imageHeight = static_cast<int>(h);
        
        scrollback.pushEntry(entry);
        // Start a fresh empty text line after the image.
        scrollback.pushLine({});
        viewport.onNewLine();
        return true;
    }
    SDL_DestroyTexture(tex);
    return false;
}


// ============================================================
// Input
// ============================================================

std::string Console::readLine() {
    if (enterPressed) {
        std::string res = currentInput;
        currentInput.clear();
        enterPressed = false;
        return res;
    }
    return "";
}

// ============================================================
// Screen
// ============================================================

void Console::clear() {
    scrollback.clear();
    scrollback.pushLine({});   // always keep one empty line
    viewport.goEnd();
    selection.clear();
}


// ============================================================
// Private helpers
// ============================================================

int Console::lineHeight() const {
    return font ? TTF_GetFontHeight(font) : 20;
}

std::string Console::lineText(std::size_t idx) const {
    if (idx >= scrollback.size()) return "";
    std::string out;
    for (const auto& seg : scrollback.lineAt(idx))
        out += seg.text;
    return out;
}

std::size_t Console::pixelToLineIndex(int y) const {
    if (scrollback.size() == 0) return SIZE_MAX;
    
    // Reverse-calculate layout to match rendering logic
    int currentY = windowHeight - 10; // Bottom of text area
    std::size_t idx = scrollback.size();
    
    std::size_t skip = std::min(viewport.scrollOffset, scrollback.size() - 1);
    idx -= skip;
    
    while (idx > 0) {
        int h = lineHeight();
        if (scrollback.entryAt(idx - 1).type == ConsoleEntry::Type::Image) {
            // Scale image width to half of window width, maintain aspect ratio
            float scaledW = windowWidth * 0.5f;
            float scaledH = scrollback.entryAt(idx - 1).imageHeight * (scaledW / scrollback.entryAt(idx - 1).imageWidth);
            h = static_cast<int>(scaledH);
        }
        
        if (currentY - h < 10) {
            currentY -= h;
            --idx;
            break;
        }
        currentY -= h;
        --idx;
    }
    
    // Now we know the first visible index `idx` starts at `currentY`.
    // Scan downwards to find which entry `y` falls into.
    std::size_t iterIdx = idx;
    int iterY = currentY;
    
    while (iterIdx < scrollback.size() - skip) {
        int h = lineHeight();
        if (scrollback.entryAt(iterIdx).type == ConsoleEntry::Type::Image) {
            float scaledW = windowWidth * 0.5f;
            float scaledH = scrollback.entryAt(iterIdx).imageHeight * (scaledW / scrollback.entryAt(iterIdx).imageWidth);
            h = static_cast<int>(scaledH);
        }
        
        if (y >= iterY && y < iterY + h) {
            return iterIdx;
        }
        
        iterY += h;
        ++iterIdx;
    }
    
    return SIZE_MAX;
}

std::size_t Console::pixelToColumn(int x, std::size_t lineIdx) const {
    if (!font || lineIdx >= scrollback.size()) return 0;
    std::string text = lineText(lineIdx);
    if (text.empty()) return 0;

    // Binary search for the column whose rendered width best matches x.
    int xOff = 10; // left padding
    int px   = x - xOff;
    if (px <= 0) return 0;

    // Walk character by character (ASCII safe).
    int accum = 0;
    for (std::size_t col = 0; col < text.size(); ++col) {
        int cw = 0;
        TTF_GetStringSize(font, text.c_str() + col, 1, &cw, nullptr);
        if (accum + cw / 2 >= px)
            return col;
        accum += cw;
    }
    return text.size();
}

void Console::copySelection() {
    if (!selection.active) return;

    std::string out;
    SelectionPos s = selection.selStart();
    SelectionPos e = selection.selEnd();

    for (std::size_t li = s.line; li <= e.line && li < scrollback.size(); ++li) {
        std::string lt = lineText(li);
        std::size_t from = (li == s.line) ? s.col : 0;
        std::size_t to   = (li == e.line) ? e.col : lt.size();
        from = std::min(from, lt.size());
        to   = std::min(to,   lt.size());
        out += lt.substr(from, to - from);
        if (li != e.line) out += '\n';
    }

    if (!out.empty())
        SDL_SetClipboardText(out.c_str());
}
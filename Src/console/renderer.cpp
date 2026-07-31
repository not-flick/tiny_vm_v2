// ============================================================
// renderer.cpp  –  Viewport-clipped rendering with selection
// ============================================================
#include "console.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// ============================================================
// draw()  –  render a UTF-8 string at (x,y) and return its width
// ============================================================
int Console::draw(
    std::string_view text,
    int x, int y,
    uint8_t r, uint8_t g, uint8_t b)
{
    if (!renderer || !font || text.empty()) return 0;

    SDL_Color color{r, g, b, 255};

    SDL_Surface* surface =
        TTF_RenderText_Blended(font, text.data(), text.size(), color);

    if (!surface) {
        SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());
        return 0;
    }

    float width  = static_cast<float>(surface->w);
    float height = static_cast<float>(surface->h);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return 0;
    }

    SDL_FRect dst{
        static_cast<float>(x),
        static_cast<float>(y),
        width, height
    };
    SDL_RenderTexture(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);

    return static_cast<int>(width);
}

// ============================================================
// drawHighlight()  –  filled semi-transparent selection rect
// ============================================================
void Console::drawHighlight(int x, int y, int w, int h)
{
    SDL_SetRenderDrawColor(renderer, 80, 160, 255, 80); // blue tint
    SDL_FRect rect{
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(w),
        static_cast<float>(h)
    };
    SDL_RenderFillRect(renderer, &rect);
}

// ============================================================
// drawCharacter()  –  single character fallback
// ============================================================
void Console::drawCharacter(char32_t character, int x, int y)
{
    char utf8[5]{};
    if (character <= 0x7F) {
        utf8[0] = static_cast<char>(character);
    } else {
        utf8[0] = '?';
    }
    draw(utf8, x, y);
}

// ============================================================
// render()  –  main rendering entry point
// ============================================================
void Console::render()
{
    // ---- Background ------------------------------------------
    SDL_SetRenderDrawColor(renderer, 18, 18, 25, 255); // dark navy
    SDL_RenderClear(renderer);

    if (scrollback.size() == 0) {
        SDL_RenderPresent(renderer);
        return;
    }

    int textTop = 10;
    int textAreaH = windowHeight - textTop;
    int lh = lineHeight();

    // Reverse-calculate layout to match rendering logic
    int currentY = windowHeight - 10; // Bottom of text area
    std::size_t idx = scrollback.size();
    
    std::size_t skip = std::min(viewport.scrollOffset, scrollback.size() - 1);
    idx -= skip;
    
    while (idx > 0) {
        int h = lh;
        if (scrollback.entryAt(idx - 1).type == ConsoleEntry::Type::Image) {
            float scaledW = windowWidth * 0.5f;
            float scaledH = scrollback.entryAt(idx - 1).imageHeight * (scaledW / scrollback.entryAt(idx - 1).imageWidth);
            h = static_cast<int>(scaledH);
        }
        
        if (currentY - h < textTop) {
            currentY -= h;
            --idx;
            break; // Starts above textTop
        }
        currentY -= h;
        --idx;
    }

    std::size_t first = idx;
    std::size_t last = scrollback.size() - skip;
    std::size_t total = scrollback.size();

    // ---- Render visible lines --------------------------------
    int rowY = currentY;
    
    for (std::size_t li = first; li < last; ++li) {
        const auto& entry = scrollback.entryAt(li);
        
        int h = lh;
        if (entry.type == ConsoleEntry::Type::Image) {
            float scaledW = windowWidth * 0.5f;
            float scaledH = entry.imageHeight * (scaledW / entry.imageWidth);
            h = static_cast<int>(scaledH);
            
            if (rowY + h > 0 && rowY < windowHeight) {
                SDL_FRect dstRect;
                dstRect.w = scaledW;
                dstRect.h = scaledH;
                dstRect.x = (windowWidth - scaledW) / 2.0f; // Center image
                dstRect.y = static_cast<float>(rowY);
                SDL_RenderTexture(renderer, entry.texture, nullptr, &dstRect);
            }
        } else {
            // Text rendering
            if (rowY + h > 0 && rowY < windowHeight) {
                const TextLine& tl = entry.textLine;
                int xOffset = 10;

                std::string flat;
                for (const auto& seg : tl) flat += seg.text;

                bool lineInSel = selection.active &&
                                 li >= selection.selStart().line &&
                                 li <= selection.selEnd().line;

                if (lineInSel && !flat.empty()) {
                    SelectionPos s = selection.selStart();
                    SelectionPos e = selection.selEnd();

                    std::size_t fromCol = (li == s.line) ? s.col : 0;
                    std::size_t toCol   = (li == e.line) ? e.col : flat.size();
                    fromCol = std::min(fromCol, flat.size());
                    toCol   = std::min(toCol,   flat.size());

                    if (fromCol < toCol) {
                        int xFrom = 10, xTo = 10;
                        if (font) {
                            int wBefore = 0, wRange = 0;
                            if (fromCol > 0)
                                TTF_GetStringSize(font, flat.c_str(), fromCol, &wBefore, nullptr);
                            if (toCol > fromCol)
                                TTF_GetStringSize(font, flat.c_str() + fromCol, toCol - fromCol, &wRange, nullptr);
                            xFrom = 10 + wBefore;
                            xTo   = xFrom + wRange;
                        }
                        drawHighlight(xFrom, rowY, xTo - xFrom, lh);
                    } else if (li != e.line) {
                        drawHighlight(10, rowY, windowWidth - 10, lh);
                    }
                }

                for (const auto& seg : tl) {
                    xOffset += draw(seg.text, xOffset, rowY, seg.r, seg.g, seg.b);
                }

                if (li == total - 1) {
                    xOffset += draw(currentInput, xOffset, rowY, 220, 220, 220);
                    if ((SDL_GetTicks() / 530) % 2 == 0) {
                        draw("|", xOffset, rowY, 100, 220, 255);
                    }
                }
            }
        }
        
        rowY += h;
    }

    // ---- Scrollbar indicator ---------------------------------
    if (viewport.scrollOffset > 0 && total > 1) {
        float barH = 50.0f; // Fixed height scrollbar for simplicity when using variable heights
        float barY = textTop + static_cast<float>(textAreaH - barH) *
                     (1.0f - static_cast<float>(viewport.scrollOffset) / static_cast<float>(total - 1));

        SDL_SetRenderDrawColor(renderer, 100, 100, 120, 180);
        SDL_FRect bar{static_cast<float>(windowWidth - 6), barY, 4.0f, barH};
        SDL_RenderFillRect(renderer, &bar);
    }

    SDL_RenderPresent(renderer);
}
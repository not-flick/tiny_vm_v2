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

    int yOffset = 10;

    // ---- Banner (optional) -----------------------------------
    if (bannerTexture && bannerWidth > 0 && bannerHeight > 0) {
        int winW = windowWidth;
        float scaledW = winW * 0.5f;
        float scaledH = (float)bannerHeight * (scaledW / (float)bannerWidth);

        SDL_FRect dstRect;
        dstRect.w = scaledW;
        dstRect.h = scaledH;
        dstRect.x = (winW - scaledW) / 2.0f;
        dstRect.y = 20.0f;
        SDL_RenderTexture(renderer, bannerTexture, nullptr, &dstRect);

        yOffset = (int)(dstRect.y + dstRect.h) + 30;
    }

    // ---- Text area metrics -----------------------------------
    int lh        = lineHeight();
    int textTop   = yOffset;
    int textAreaH = windowHeight - textTop;

    std::size_t visLines = (lh > 0)
                           ? static_cast<std::size_t>(textAreaH / lh)
                           : 25u;

    std::size_t total = scrollback.size();
    std::size_t first = viewport.firstVisible(total, visLines);
    std::size_t last  = std::min(first + visLines, total);

    // ---- Render visible lines --------------------------------
    for (std::size_t li = first; li < last; ++li) {
        int rowY = textTop + static_cast<int>(li - first) * lh;

        // Skip rows that are off-screen.
        if (rowY + lh < 0 || rowY > windowHeight) {
            continue;
        }

        const TextLine& tl = scrollback.lineAt(li);
        int xOffset = 10;

        // Build the flat text of this line so we can measure column offsets.
        std::string flat;
        for (const auto& seg : tl) flat += seg.text;

        // Pre-compute per-character x positions for selection hit-testing.
        // (We only need this when the line is inside the selection range.)
        bool lineInSel = selection.active &&
                         li >= selection.selStart().line &&
                         li <= selection.selEnd().line;

        // ---- Selection background for this line --------------
        if (lineInSel && !flat.empty()) {
            SelectionPos s = selection.selStart();
            SelectionPos e = selection.selEnd();

            std::size_t fromCol = (li == s.line) ? s.col : 0;
            std::size_t toCol   = (li == e.line) ? e.col : flat.size();
            fromCol = std::min(fromCol, flat.size());
            toCol   = std::min(toCol,   flat.size());

            if (fromCol < toCol) {
                // Measure x for fromCol and toCol.
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
                // Full-line highlight for middle lines.
                drawHighlight(10, rowY, windowWidth - 10, lh);
            }
        }

        // ---- Draw text segments ------------------------------
        for (const auto& seg : tl) {
            xOffset += draw(seg.text, xOffset, rowY, seg.r, seg.g, seg.b);
        }

        // ---- Draw input prompt on the last line --------------
        if (li == total - 1) {
            xOffset += draw(currentInput, xOffset, rowY, 220, 220, 220);
            // Blinking cursor.
            if ((SDL_GetTicks() / 530) % 2 == 0) {
                draw("|", xOffset, rowY, 100, 220, 255);
            }
        }
    }

    // ---- Scrollbar indicator ---------------------------------
    if (viewport.scrollOffset > 0 && total > visLines) {
        // Draw a slim scrollbar on the right edge.
        float barH = static_cast<float>(textAreaH) *
                     (static_cast<float>(visLines) / static_cast<float>(total));
        float barY = textTop + static_cast<float>(textAreaH - barH) *
                     (1.0f - static_cast<float>(viewport.scrollOffset) /
                             static_cast<float>(Viewport::maxOffset(total, visLines)));

        SDL_SetRenderDrawColor(renderer, 100, 100, 120, 180);
        SDL_FRect bar{static_cast<float>(windowWidth - 6), barY, 4.0f, barH};
        SDL_RenderFillRect(renderer, &bar);
    }

    SDL_RenderPresent(renderer);
}
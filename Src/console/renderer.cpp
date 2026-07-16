#include "console.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

int Console::draw(
    std::string_view text,
    int x,
    int y,
    uint8_t r,
    uint8_t g,
    uint8_t b)
{
    if (!renderer || !font || text.empty())
        return 0;

    SDL_Color color{r, g, b, 255};

    SDL_Surface* surface =
        TTF_RenderText_Blended(
            font,
            text.data(),
            text.size(),
            color);

    if (!surface)
    {
        SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());
        return 0;
    }

    float width  = static_cast<float>(surface->w);
    float height = static_cast<float>(surface->h);

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            renderer,
            surface);

    SDL_DestroySurface(surface);

    if (!texture)
    {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return 0;
    }

    SDL_FRect dst{
        static_cast<float>(x),
        static_cast<float>(y),
        width,
        height
    };

    SDL_RenderTexture(
        renderer,
        texture,
        nullptr,
        &dst);

    SDL_DestroyTexture(texture);
    
    return static_cast<int>(width);
}

void Console::drawCharacter(
    char32_t character,
    int x,
    int y)
{
    char utf8[5]{};

    if (character <= 0x7F)
    {
        utf8[0] = static_cast<char>(character);
        utf8[1] = '\0';
    }
    else
    {
        // Proper UTF-8 encoding can be added later.
        utf8[0] = '?';
        utf8[1] = '\0';
    }

    draw(utf8, x, y);
}

void Console::render()
{
    SDL_SetRenderDrawColor(
        renderer,
        30,
        30,
        30,
        255);

    SDL_RenderClear(renderer);

    int yOffset = 10;
    int lineHeight = font ? TTF_GetFontHeight(font) : 20;

    for (const auto& line : textLines) {
        if (!line.empty()) {
            int xOffset = 10;
            for (const auto& seg : line) {
                xOffset += draw(seg.text, xOffset, yOffset, seg.r, seg.g, seg.b);
            }
        }
        yOffset += lineHeight;
    }

    SDL_RenderPresent(renderer);
}
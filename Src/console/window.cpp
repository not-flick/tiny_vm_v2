#include "console.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

Console::Console(int width, int height, std::string_view title)
{
    windowWidth = width;
    windowHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL Init Failed: %s", SDL_GetError());
        running = false;
        return;
    }

    if (!TTF_Init())
    {
        SDL_Log("TTF Init Failed: %s", SDL_GetError());

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
        SDL_Log("Window Creation Failed: %s", SDL_GetError());

        TTF_Quit();
        SDL_Quit();

        running = false;
        return;
    }

    renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer)
    {
        SDL_Log("Renderer Creation Failed: %s", SDL_GetError());

        SDL_DestroyWindow(window);

        TTF_Quit();
        SDL_Quit();

        running = false;
        return;
    }

    running = true;
}

Console::~Console()
{
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
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
                break;
        }
    }
}

void Console::present()
{
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}
#include "BootLogo.h"

BootLogo::~BootLogo() {
    unload();
}

bool BootLogo::load(SDL_Renderer* renderer, const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        return false;
    }

    m_texture = IMG_LoadTexture(renderer, path.string().c_str());
    if (!m_texture) {
        SDL_Log("BootLogo failed to load texture: %s", SDL_GetError());
        return false;
    }

    float w, h;
    if (SDL_GetTextureSize(m_texture, &w, &h)) {
        m_width = static_cast<int>(w);
        m_height = static_cast<int>(h);
    } else {
        SDL_Log("BootLogo failed to get texture size: %s", SDL_GetError());
        unload();
        return false;
    }

    return true;
}

void BootLogo::show(SDL_Renderer* renderer, SDL_Window* window, int duration_ms) {
    if (!m_texture || !renderer || !window) {
        return;
    }

    uint64_t start = SDL_GetTicks();
    SDL_Event event;
    
    while (SDL_GetTicks() - start < static_cast<uint64_t>(duration_ms)) {
        // Pump events to keep the window responsive.
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                // If user closes window during splash, just return to let standard teardown happen.
                return;
            }
        }

        int winW = 0, winH = 0;
        SDL_GetWindowSize(window, &winW, &winH);

        // Scale to roughly 30% of the window size, preserving aspect ratio.
        float targetRatio = 0.30f;
        float scaledW = static_cast<float>(winW) * targetRatio;
        float scaleFactor = scaledW / static_cast<float>(m_width);
        float scaledH = static_cast<float>(m_height) * scaleFactor;

        // Ensure we don't scale up past the window height
        if (scaledH > winH * targetRatio) {
            scaledH = winH * targetRatio;
            scaleFactor = scaledH / static_cast<float>(m_height);
            scaledW = static_cast<float>(m_width) * scaleFactor;
        }

        SDL_FRect dstRect;
        dstRect.w = scaledW;
        dstRect.h = scaledH;
        dstRect.x = (winW - scaledW) / 2.0f;
        dstRect.y = (winH - scaledH) / 2.0f;

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, m_texture, nullptr, &dstRect);
        SDL_RenderPresent(renderer);
    }
}

void BootLogo::unload() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

#include "render/internal.hh"
#include "render/window.hh"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_log.h>

namespace {
    SDL_Window *window;
    SDL_Renderer *renderer;
}

SDL_Renderer *get_renderer() {
    return renderer;
}

bool open_window(WindowConfig config) {
    window = SDL_CreateWindow(config.title, config.width, config.height, 0);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void close_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}


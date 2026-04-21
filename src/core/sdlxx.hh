#pragma once
#include <SDL3/SDL_render.h>
#include <memory>

using Color = SDL_Color;
using Rectangle = SDL_FRect;

struct DestroySurface {
    void operator()(SDL_Surface *surface) const { SDL_DestroySurface(surface); }
};

struct DestroyTexture {
    void operator()(SDL_Texture *texture) const { SDL_DestroyTexture(texture); }
};

struct DestroyRenderer {
    void operator()(SDL_Renderer *renderer) const { SDL_DestroyRenderer(renderer); }
};

struct DestroyWindow {
    void operator()(SDL_Window *window) const { SDL_DestroyWindow(window); }
};

using Surface = std::unique_ptr<SDL_Surface, DestroySurface>;
using Texture = std::unique_ptr<SDL_Texture, DestroyTexture>;
using Renderer = std::unique_ptr<SDL_Renderer, DestroyRenderer>;
using Window = std::unique_ptr<SDL_Window, DestroyWindow>;


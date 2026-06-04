#include "engine/render/draw.hh"
#include "internal.hh"
#include <SDL3/SDL_render.h>

void draw_rectangle(SDL_FRect rect, SDL_Color color) {
    auto renderer = detail::get_renderer();
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}


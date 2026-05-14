#pragma once
#include "engine/core/types.hh"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>

using Color = SDL_Color;
using Rectangle = SDL_FRect;

constexpr SDL_Color hex_color(u32 rgb, u8 alpha = 255) {
    return { u8(rgb >> 16), u8(rgb >> 8), u8(rgb), alpha };
}

void draw_rectangle(SDL_FRect rect, SDL_Color color);


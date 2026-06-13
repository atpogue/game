#pragma once
#include "core/types.hh"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>

using Color     = SDL_Color;
using Rectangle = SDL_FRect;

constexpr SDL_Color hex_color(u32 rgba)
{
  return {u8(rgba >> 24), u8(rgba >> 16), u8(rgba >> 8), u8(rgba)};
}

void draw_rectangle(SDL_FRect rect, SDL_Color color);


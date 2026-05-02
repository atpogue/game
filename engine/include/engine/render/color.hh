#pragma once
#include "engine/core/types.hh"
#include <SDL3/SDL_pixels.h>

constexpr SDL_Color hex_color(u32 rgb, u8 alpha = 255) {
    return { u8(rgb >> 16), u8(rgb >> 8), u8(rgb), alpha };
}


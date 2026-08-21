#pragma once
#include "core/types.hh"

struct Color
{
  u8 r, g, b, a;
};

constexpr Color make_color_hex(u32 rgba)
{
  return { u8(rgba >> 24), u8(rgba >> 16), u8(rgba >> 8), u8(rgba) };
}


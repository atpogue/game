#pragma once
#include "core/result.hh"
#include "game/types.hh"
#include "gfx/color.hh"
#include "gfx/rectangle.hh"
#include <string_view>

struct TextureDef;
struct LuaTable;

struct Sprite
{
  Token<TextureDef> atlas;
  Rectangle         source;
  Color             tint;
};

Result<Sprite> parse_sprite(LuaTable const& table);

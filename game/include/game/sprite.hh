#pragma once
#include "core/error.hh"
#include "game/types.hh"
#include "gfx/color.hh"
#include "gfx/rectangle.hh"
#include <string_view>

struct TextureDef;
struct lua_State;

struct Sprite
{
  Token<TextureDef> atlas;
  Rectangle         source;
  Color             tint;
};

namespace lua {
  Result<Sprite>
  try_get_sprite(CatalogWriter catalog, lua_State* L, int idx, std::string_view field);
}

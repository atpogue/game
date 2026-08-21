#pragma once
#include "core/error.hh"
#include "gfx/color.hh"
#include "gfx/rectangle.hh"
#include "types.hh"

struct TextureAsset;
struct lua_State;

struct Sprite
{
  Token<TextureAsset> atlas;
  Rectangle           source;
  Color               tint;
  // assumes the sprite is valid
  void draw(float x, float y, float scale = 1.f) const;
};

namespace lua {
  Result<Sprite>
  try_get_sprite(CatalogWriter catalog, lua_State* L, int idx, std::string_view field);
}

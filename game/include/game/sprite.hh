#pragma once
#include "core/result.hh"
#include "game/types.hh"
#include "gfx/color.hh"
#include "gfx/rectangle.hh"

struct TextureDef;
struct LuaNode;

struct Sprite
{
  Token<TextureDef> atlas;
  Rectangle         source;
  Color             tint;
};

[[nodiscard]] Result<Sprite> parse_sprite(LuaNode const& node, CatalogWriter catalog);

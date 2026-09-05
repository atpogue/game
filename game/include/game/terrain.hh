#pragma once
#include "core/result.hh"
#include "game/sprite.hh"
#include "game/types.hh"

struct LuaNode;

struct Terrain
{
  Sprite sprite;
};

[[nodiscard]] Result<Terrain> parse_terrain(LuaNode const& node, CatalogWriter catalog);

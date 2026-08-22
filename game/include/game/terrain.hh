#pragma once
#include "game/sprite.hh"
#include "game/types.hh"

struct lua_State;

struct Terrain
{
  Sprite sprite;
};

namespace lua { void add_terrain_builder(lua_State* L, CatalogWriter& catalog); }

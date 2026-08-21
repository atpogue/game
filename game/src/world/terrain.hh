#pragma once
#include "sprite.hh"
#include "types.hh"

struct lua_State;

struct Terrain
{
  Sprite sprite;
};

namespace lua { void add_terrain_builder(lua_State* L, CatalogWriter& catalog); }

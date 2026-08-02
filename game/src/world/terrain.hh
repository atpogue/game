#pragma once
#include "sprite.hh"
#include "types.hh"

struct Terrain
{
  Sprite sprite;
};

struct lua_State;

namespace lua { void add_terrain_builder(lua_State* L, CatalogWriter& catalog); }

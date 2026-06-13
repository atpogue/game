#pragma once
#include "sprite.hh"
#include <string>

struct Terrain
{
  std::string name;
  Sprite      sprite;
};

struct lua_State;
struct Codex;

namespace lua { void add_terrain_builder(lua_State* L, Codex& codex); }


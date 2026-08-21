#pragma once
#include "core/types.hh"

struct Terrain;

struct Tile
{
  Token<Terrain> terrain;
  // u32 elevation;
  // u32 structure;
};

#pragma once
#include "core/types.hh"
#include "game/content/terrain.hh"

namespace Game {
  struct Tile
  {
    Handle<Terrain> terrain;
    // u32 elevation;
    // u32 structure;
  };
}

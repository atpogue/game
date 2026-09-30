#pragma once
#include "game/chunk.hh"

namespace Game {
  struct World;

  struct GrasslandGenerator : ChunkGenerator
  {
    GrasslandGenerator(World const&, u64 seed);
    void generate(u32 x, u32 y, Chunk& chunk) override;

  private:

    u64 const             seed_;
    Handle<Terrain> const terrain_[6];
  };
}

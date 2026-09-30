#pragma once
#include "core/grid2.hh"
#include "game/tile.hh"

namespace Game {
  constexpr u32 chunk_size = 64;

  struct Chunk : Grid2<Tile, chunk_size, chunk_size>
  {
    // other relevant data...
  };

  struct ChunkGenerator
  {
    virtual ~ChunkGenerator()                         = default;
    virtual void generate(u32 x, u32 y, Chunk& chunk) = 0;
  };

  struct NullChunkGenerator final : ChunkGenerator
  {
    void generate(u32, u32, Chunk&) override {};
  };

  struct ChunkLoader
  {
    virtual ~ChunkLoader()                        = default;
    virtual void load(u32 x, u32 y, Chunk& chunk) = 0;
  };

  struct NullChunkLoader final : ChunkLoader
  {
    void load(u32, u32, Chunk&) override {};
  };
}

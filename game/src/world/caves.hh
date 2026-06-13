#pragma once
#include "world/chunk.hh"

#include "engine/core/grid2.hh"
#include "engine/core/types.hh"

// cellular automata rule used for cave generation
void generate_cave(Grid2<u32>& out, u32 wall, u32 floor, u32 birth, u32 survival, u32 range,
                   u32 iterations);

struct CaveGenerator : ChunkGenerator
{
  CaveGenerator(const Context ctx, u64 seed);
  void generate(u32 x, u32 y, Chunk& chunk) override;

private:

  u64 seed_;
  u32 wall_, floor_;
};


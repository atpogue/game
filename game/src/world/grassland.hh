#pragma once
#include "world/chunk.hh"

struct Terrain;

struct GrasslandGenerator : ChunkGenerator
{
  GrasslandGenerator(Context const ctx, u64 seed);
  void generate(u32 x, u32 y, Chunk& chunk) override;

private:

  u64 const            seed_;
  Token<Terrain> const terrain_[6];
};

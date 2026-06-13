#pragma once
#include "world/chunk.hh"

struct GrasslandGenerator : ChunkGenerator
{
  GrasslandGenerator(Context const ctx, u64 seed);
  void generate(u32 x, u32 y, Chunk& chunk) override;

private:

  u64 const seed_;
  u32 const terrain_[6];
};

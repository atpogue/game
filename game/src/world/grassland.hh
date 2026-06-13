#pragma once
#include "world/chunk.hh"

struct GrasslandGenerator : ChunkGenerator
{
  GrasslandGenerator(const Context ctx, u64 seed);
  void generate(u32 x, u32 y, Chunk& chunk) override;

private:

  const u64 seed_;
  const u32 terrain_[6];
};


#pragma once
#include "core/grid2.hh"
#include "world/tile.hh"
struct Camera;
struct Context;
constexpr u32 chunk_size = 64;

struct Chunk : Grid2<Tile, chunk_size, chunk_size>
{
  // other relevant data...
  void render(Context const ctx, Camera const& camera, float tile_size) const;
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

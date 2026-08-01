#include "context.hh"
#include "core/random.hh"
#include "world/grassland.hh"
#include <random>

GrasslandGenerator::GrasslandGenerator(Context const ctx, u64 seed)
  : seed_{seed}
  , terrain_{
      ctx.catalog.find<Terrain>("grass-1"), ctx.catalog.find<Terrain>("grass-2"),
      ctx.catalog.find<Terrain>("grass-3"), ctx.catalog.find<Terrain>("grass-tall"),
      ctx.catalog.find<Terrain>("dirt"),    ctx.catalog.find<Terrain>("rocks"),
    }
{
  for (auto token : terrain_) { PRECONDITION(token, "terrain undefined"); }
}

void GrasslandGenerator::generate(u32 x, u32 y, Chunk& chunk)
{
  auto const                         hash = split_mix(seed_ ^ split_mix((u64{x} << 32) | y));
  Xoshiro256ss                       rng{hash};
  std::uniform_int_distribution<u32> distribution{0u, (u32)std::size(terrain_) - 1u};

  for (auto& tile : chunk) { tile.terrain = terrain_[distribution(rng)]; }
}

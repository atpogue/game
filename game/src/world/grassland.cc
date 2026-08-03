#include "world/grassland.hh"
#include "catalog.hh"
#include "context.hh"
#include "core/random.hh"
#include <random>

Token<Terrain> find_terrain(Context ctx, std::string_view label)
{
  return access_catalog(ctx).find<Terrain>(label);
}

GrasslandGenerator::GrasslandGenerator(Context const ctx, u64 seed)
  : seed_{seed}
  , terrain_{
      find_terrain(ctx, "grass-1"),    find_terrain(ctx, "grass-2"), find_terrain(ctx, "grass-3"),
      find_terrain(ctx, "grass-tall"), find_terrain(ctx, "dirt"),    find_terrain(ctx, "rocks"),
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

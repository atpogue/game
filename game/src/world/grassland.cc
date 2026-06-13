#include "world/grassland.hh"

#include "context.hh"

#include "engine/core/random.hh"

#include <random>

GrasslandGenerator::GrasslandGenerator(const Context ctx, u64 seed)
  : seed_{seed}
  , terrain_{
      ctx.codex.terrain.find("grass-1"), ctx.codex.terrain.find("grass-2"),
      ctx.codex.terrain.find("grass-3"), ctx.codex.terrain.find("grass-tall"),
      ctx.codex.terrain.find("dirt"),    ctx.codex.terrain.find("rocks"),
    }
{
  for (auto id : terrain_) { PRECONDITION(id != nil, "terrain undefined"); }
}

void GrasslandGenerator::generate(u32 x, u32 y, Chunk& chunk)
{
  const auto                         hash = split_mix(seed_ ^ split_mix((u64{x} << 32) | y));
  Xoshiro256ss                       rng{hash};
  std::uniform_int_distribution<u32> distribution{0u, (u32)std::size(terrain_) - 1u};
  for (auto& tile : chunk) { tile.terrain = terrain_[distribution(rng)]; }
}


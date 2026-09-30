#include "game/biome/grassland.hh"
#include "core/random.hh"
#include "game/world.hh"
#include <random>

namespace Game {
  GrasslandGenerator::GrasslandGenerator(World const& world, u64 seed)
    : seed_{ seed }
    , terrain_{
      world.content.find<Terrain>("grass-1"), world.content.find<Terrain>("grass-2"),
      world.content.find<Terrain>("grass-3"), world.content.find<Terrain>("grass-tall"),
      world.content.find<Terrain>("dirt"),    world.content.find<Terrain>("rocks"),
    }
  {
    for (Handle<Terrain> terrain : terrain_) {
      PRECONDITION(terrain, "terrain undefined");
    }
  }

  void GrasslandGenerator::generate(u32 x, u32 y, Chunk& chunk)
  {
    auto const                         hash = split_mix(seed_ ^ split_mix((u64{ x } << 32) | y));
    Xoshiro256ss                       rng{ hash };
    std::uniform_int_distribution<u32> distribution{ 0u, (u32)std::size(terrain_) - 1u };

    for (Tile& tile : chunk) {
      tile.terrain = terrain_[distribution(rng)];
    }
  }
}

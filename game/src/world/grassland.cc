#include "engine/core/random.hh"
#include "world/grassland.hh"
#include "world/terrain.hh"
#include <random>

GrasslandGenerator::GrasslandGenerator(u64 seed, const Catalog<Terrain> &terrain) 
    : seed_{seed}
    , terrain_{
        terrain.find("grass-1"),
        terrain.find("grass-2"),
        terrain.find("grass-3"),
        terrain.find("grass-tall"),
        terrain.find("dirt"),
        terrain.find("rocks"),
    }
{
    for (auto id : terrain_) {
        PRECONDITION(id != nil, "terrain undefined");
    }
}

void GrasslandGenerator::generate(u32 x, u32 y, Chunk& chunk) {
    const auto hash = split_mix(seed_ ^ split_mix((u64{x} << 32) | y));
    Xoshiro256ss rng{hash};
    std::uniform_int_distribution<u32> distribution{0u, (u32)std::size(terrain_)-1u};
    for (auto &tile : chunk) {
        tile.terrain = terrain_[distribution(rng)];
    }
}


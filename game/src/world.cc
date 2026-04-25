#include "world.hh"
#include "engine/core/invariant.hh"
#include "engine/core/random.hh"
#include "engine/world/terrain.hh"
#include <random>

namespace {
    constexpr int terrain_count = 6;
}

MyChunkGenerator::MyChunkGenerator(u64 seed) 
    : seed_{seed}
{}

void MyChunkGenerator::generate(u32 x, u32 y, Chunk& chunk) {
    Xoshiro256ss rng{split_mix(seed_ ^ split_mix((u64{x} << 32) | y))};

    const char *names[terrain_count]{
        "grass-1", "grass-2", "grass-3", "grass-tall",
        "dirt", "rocks"
    };

    std::uniform_int_distribution<int> distribution{0, terrain_count-1};
    Terrain::Id terrain[terrain_count];

    for (int i = 0; i < terrain_count; i++) {
        auto id = terrains::find(names[i]); 
        INVARIANT(id, "terrain undefined");
        terrain[i] = *id;
    }

    for (auto &tile : chunk) {
        tile.terrain = terrain[distribution(rng)];
    }
}


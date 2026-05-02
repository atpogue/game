#include "world.hh"
#include "engine/core/invariant.hh"
#include "engine/core/random.hh"
#include "engine/world/terrain.hh"
#include <ranges>
#include <random>

MyChunkGenerator::MyChunkGenerator(u64 seed) 
    : seed_{seed}
{}

void MyChunkGenerator::generate(u32 x, u32 y, Chunk& chunk) {
    const auto hash = split_mix(seed_ ^ split_mix((u64{x} << 32) | y));
    Xoshiro256ss rng{hash};

    const char *names[]{
        "grass-1", "grass-2", "grass-3", "grass-tall",
        "dirt", "rocks"
    };
    constexpr auto terrain_count = std::size(names);
    Terrain::Id terrain[terrain_count];
    std::uniform_int_distribution<int> distribution{0, terrain_count-1};

    for (auto [name, id] : std::views::zip(names, terrain)) {
        auto found = terrains::find(name);
        INVARIANT(found, "terrain undefined");
        id = *found;
    }

    for (auto &tile : chunk) {
        tile.terrain = terrain[distribution(rng)];
    }
}


#include "engine/core/random.hh"
#include "world/grassland.hh"
#include "world/terrain.hh"
#include <ranges>
#include <random>

GrasslandGenerator::GrasslandGenerator(u64 seed) 
    : seed_{seed}
{}

void GrasslandGenerator::generate(u32 x, u32 y, Chunk& chunk) {
    const auto hash = split_mix(seed_ ^ split_mix((u64{x} << 32) | y));
    Xoshiro256ss rng{hash};

    const char *names[]{
        "grass-1", "grass-2", "grass-3", "grass-tall",
        "dirt", "rocks"
    };
    constexpr auto terrain_count = std::size(names);
    u32 terrain[terrain_count];

    for (auto [name, id] : std::views::zip(names, terrain)) {
        auto found = find_terrain(name);
        PRECONDITION(found != nil, "terrain undefined");
        id = found;
    }

    std::uniform_int_distribution<int> distribution{0, terrain_count-1};
    for (auto &tile : chunk) {
        tile.terrain = terrain[distribution(rng)];
    }
}


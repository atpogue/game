#include "world.hh"
#include "procgen.hh"
#include "engine/core/invariant.hh"
#include "engine/core/random.hh"
#include "engine/world/terrain.hh"
#include <ranges>

MyChunkGenerator::MyChunkGenerator(u64 seed) 
    : seed_{seed}
{}

void MyChunkGenerator::generate(u32 x, u32 y, Chunk& chunk) {
    const auto hash = split_mix(seed_ ^ split_mix((u64{x} << 32) | y));
    Xoshiro256ss rng{hash};

    const char *names[]{ "dirt", "rocks" };
    Terrain::Id terrain[std::size(names)];
    for (auto [id, name] : std::views::zip(terrain, names)) {
        auto found = terrains::find(name);
        INVARIANT(found, "terrain undefined");
        id = *found;
    }

    Grid2<u8> cave(chunk_size, chunk_size);
    random_fill(cave, hash, 0.4f);
    generate_caves(cave, 1, 5, 1);
    generate_caves(cave, 2, 3, 2);
    generate_caves(cave, 1, 5, 1);
    //generate_caves(cave, 2, 6, 1);
    for (u32 x = 0; x < chunk_size; x++) {
        for (u32 y = 0; y < chunk_size; y++) {
            chunk[x, y].terrain = terrain[cave[x, y]];
        }
    }
}


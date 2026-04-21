#include "chunk.hh"
#include "terrain.hh"
#include "core/random.hh"
#include <random>

// procedural generation using world seed
bool generate_chunk(u64 seed, u32 x, u32 y, Chunk& chunk) {
    Xoshiro256ss rng{split_mix(seed ^ split_mix((u64{x} << 32) | y))};
    std::uniform_int_distribution<int> dice{0, 6};
    Terrain::Id terr[6];
    if (auto id = terrain::find("grass-1"); id) terr[0] = *id;
    else return false; // terrain not defined
    if (auto id = terrain::find("grass-2"); id) terr[1] = *id;
    else return false;
    if (auto id = terrain::find("grass-3"); id) terr[2] = *id;
    else return false;
    if (auto id = terrain::find("grass-tall"); id) terr[3] = *id;
    else return false;
    if (auto id = terrain::find("dirt"); id) terr[4] = *id;
    else return false;
    if (auto id = terrain::find("rocks"); id) terr[5] = *id;
    else return false;
    for (auto &tile : chunk) {
        tile.terrain = terr[dice(rng)];
    }
    return true;
}

// load and apply deltas if there are any
void load_chunk(u32 x, u32 y, Chunk &chunk) {
}


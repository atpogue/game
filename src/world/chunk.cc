#include "chunk.hh"
#include "terrain.hh"
#include "core/random.hh"
#include <random>

// procedural generation using world seed
bool generate_chunk(u64 seed, u32 x, u32 y, Chunk& chunk) {
    Xoshiro256ss rng{split_mix(seed ^ split_mix((u64{x} << 32) | y))};
    std::bernoulli_distribution coin{0.5};
    Terrain::Id grass, dirt;
    if (auto id = terrain::find("grass"); id) grass = *id;
    else return false; // terrain not defined
    if (auto id = terrain::find("dirt"); id) dirt = *id;
    else return false;
    for (auto &tile : chunk.tiles) {
        tile.terrain = coin(rng) ? grass : dirt;
    }
    return true;
}

// load and apply deltas if there are any
void load_chunk(u32 x, u32 y, Chunk &chunk) {
}


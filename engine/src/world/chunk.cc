#include "world/chunk.hh"
#include "world/terrain.hh"
#include "core/random.hh"
#include "render/camera.hh"
#include <random>

// procedural generation using world seed
bool generate_chunk(u64 seed, u32 x, u32 y, Chunk& chunk) {
    Xoshiro256ss rng{split_mix(seed ^ split_mix((u64{x} << 32) | y))};
    std::uniform_int_distribution<int> dice{0, 6};
    Terrain::Id terr[6];
    if (auto id = terrains::find("grass-1"); id) terr[0] = *id;
    else return false; // terrain not defined
    if (auto id = terrains::find("grass-2"); id) terr[1] = *id;
    else return false;
    if (auto id = terrains::find("grass-3"); id) terr[2] = *id;
    else return false;
    if (auto id = terrains::find("grass-tall"); id) terr[3] = *id;
    else return false;
    if (auto id = terrains::find("dirt"); id) terr[4] = *id;
    else return false;
    if (auto id = terrains::find("rocks"); id) terr[5] = *id;
    else return false;
    for (auto &tile : chunk) {
        tile.terrain = terr[dice(rng)];
    }
    return true;
}

// load and apply deltas if there are any
void load_chunk(u32 x, u32 y, Chunk &chunk) {
}

void Chunk::render(const Camera &camera, float tile_size) const {
    for (auto coord : camera) {
        u32 x = coord.x, y = coord.y;
        auto tile = get(x, y);
        if (!tile) continue;
        auto pixel = camera.view_coord_at({x,y}) * tile_size;
        terrains::get(tile->terrain).sprite.draw(pixel.x, pixel.y, camera.zoom);
    }
}


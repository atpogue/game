#pragma once
#include "core/grid2.hh"
#include "world/tile.hh"

constexpr u32 chunk_size = 64;

struct Chunk {
    Grid2<Tile, chunk_size, chunk_size> tiles;
    // other relevant data...
};

// procedural generation using world seed
bool generate_chunk(u64 seed, u32 x, u32 y, Chunk& chunk);

// load and apply deltas if there are any
void load_chunk(u32 x, u32 y, Chunk &chunk);


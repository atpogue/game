#pragma once
#include "core/grid2.hh"
#include "world/tile.hh"

constexpr u32 chunk_size = 64;

struct Chunk {
    Grid2<Tile, chunk_size, chunk_size> tiles;
    // other relevant data...
};

void load_chunk(u64 key, Chunk &chunk);


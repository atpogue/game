#pragma once
#include "world/chunk.hh"
#include "world/terrain.hh"

struct GrasslandGenerator : ChunkGenerator {
    GrasslandGenerator(u64 seed, const Catalog<Terrain> &terrain);
    void generate(u32 x, u32 y, Chunk &chunk) override;
private:
    const u64 seed_;
    const u32 terrain_[6];
};


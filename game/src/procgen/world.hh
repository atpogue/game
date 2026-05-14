#pragma once
#include "engine/world/chunk.hh"

struct WorldGenerator : ChunkGenerator {
    WorldGenerator(u64 seed);
    void generate(u32 x, u32 y, Chunk &chunk) override;
private:
    const u64 seed_;
};


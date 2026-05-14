#pragma once
#include "engine/core/types.hh"
#include "engine/core/grid2.hh"
#include "engine/world/chunk.hh"

// cellular automata rule used for cave generation
void generate_cave(Grid2<u32> &out, u32 wall, u32 floor,
                   u32 birth, u32 survival, u32 range, u32 iterations);

struct CaveGenerator : ChunkGenerator {
    CaveGenerator(u64 seed);
    void generate(u32 x, u32 y, Chunk &chunk) override;
private:
    const u64 seed_;
    const u32 wall_, floor_;
};


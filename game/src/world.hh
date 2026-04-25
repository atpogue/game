#pragma once
#include "world/chunk.hh"

struct MyChunkGenerator : ChunkGenerator {
    MyChunkGenerator(u64 seed);
    void generate(u32 x, u32 y, Chunk &chunk) override;
private:
    const u64 seed_;
};


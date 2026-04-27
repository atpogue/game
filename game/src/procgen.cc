#include "engine/core/grid2.hh"
#include "engine/core/random.hh"
#include "procgen.hh"
#include <random>

void random_fill(Grid2<u8> &out, u64 seed, f32 chance) {
    Xoshiro256ss rng{seed};
    std::bernoulli_distribution coin{chance};
    for (auto &tile : out) tile = coin(rng);
}

void generate_caves(Grid2<u8> &out, i64 iterations, i64 threshold, i64 range) {
    Grid2<u8> scratch(out.width(), out.height());

    // 1. fill map randomly (perlin noise)
    // 2. tile becomes wall if 3x3 locale has 5+ walls
    // B678/S345678 rule (born with 6/7/8 neighbors, survives with 3–8)?

    static const auto neighbor_count = [range](const Grid2<u8> &g, u32 x, u32 y) {
        i64 n = 0;
        for (i64 ny = i64{y}-range; ny <= i64{y}+range; ++ny) {
            for (i64 nx = i64{x}-range; nx <= i64{x}+range; ++nx) {
                if (nx < 0 || ny < 0 || !g.has(nx, ny)) {
                    // out of bounds
                    ++n;
                    continue;
                }
                n += g[nx, ny];
            }
        }
        return n;
    };

    for (i64 i = 0; i < iterations; ++i) {
        for (u32 y = 0; y < out.height(); ++y) {
            for (u32 x = 0; x < out.width(); ++x) {
                scratch[x, y] = (neighbor_count(out, x, y) >= threshold);
            }
        }
        std::swap(out, scratch);
    }
}


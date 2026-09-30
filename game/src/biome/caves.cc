#include "game/biome/caves.hh"
#include "core/grid2.hh"
#include "core/random.hh"
#include "game/world.hh"
#include <random>
#include <ranges>
#include <utility>

namespace Game {
  void generate_cave(
    Grid2<u32>& out, u32 wall, u32 floor, u32 birth, u32 survival, u32 range, u32 iterations)
  {
    DEBUG_ASSERT(out.size() > 0u);
    DEBUG_ASSERT(iterations > 0u);
    DEBUG_ASSERT(range > 0u);
    Grid2<u32> buffer(out.width(), out.height());
    auto const count_walls = [range, wall, &out](i64 x, i64 y) {
      u32 n = 0;
      for (i64 ny = y - range; ny <= y + range; ++ny) {
        for (i64 nx = x - range; nx <= x + range; ++nx) {
          if (nx < 0 || ny < 0 || !out.has(nx, ny)) {
            // out of bounds
            ++n;
            continue;
          }
          n += (out[nx, ny] == wall);
        }
      }
      return n;
    };
    while (iterations > 0u) {
      for (u32 y = 0u; y < out.height(); ++y) {
        for (u32 x = 0u; x < out.width(); ++x) {
          u32  count   = count_walls(x, y);
          u32& tile    = buffer[x, y];
          bool is_wall = (tile == wall);
          if (is_wall) {
            is_wall = (count >= survival);
          } else {
            is_wall = (count >= birth);
          }
          tile = is_wall ? wall : floor;
        }
      }
      std::swap(out, buffer);
      iterations--;
    }
  }

  CaveGenerator::CaveGenerator(World const& world, u64 seed)
    : seed_{ seed }
    , wall_{ world.content.find<Terrain>("stone") }
    , floor_{ world.content.find<Terrain>("dirt") }
  {
    PRECONDITION(wall_, "terrain undefined");
    PRECONDITION(floor_, "terrain undefined");
  }

  void CaveGenerator::generate(u32 x, u32 y, Chunk& chunk)
  {
    auto const    hash  = split_mix(seed_ ^ split_mix((u64{ x } << 32) | y));
    constexpr u32 wall  = 1u;
    constexpr u32 floor = 0u;
    Grid2<u32>    cave(chunk_size, chunk_size);
    // uniformly random fill
    Xoshiro256ss                rng{ hash };
    std::bernoulli_distribution coin{ 0.45f };
    for (auto& tile : cave)
      tile = coin(rng) ? wall : floor;
    generate_cave(cave, wall, floor, 5, 6, 1, 1);
    generate_cave(cave, wall, floor, 3, 4, 2, 2);
    generate_cave(cave, wall, floor, 9, 10, 2, 1);
    for (auto [tile, cell] : std::views::zip(chunk, cave)) {
      tile.terrain = cell == wall ? wall_ : floor_;
    }
  }
}

// TODO: perlin noise
// procedural generation as a series of transformations


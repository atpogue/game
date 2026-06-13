#include "world/caves.hh"
#include "context.hh"
#include "core/grid2.hh"
#include "core/random.hh"
#include <random>
#include <ranges>
#include <utility>

namespace { /////////////////////////////////////////////////////////////////////

} // namespace

/*
Sprite make_sprite_from_tilesheet(u32 tx, u32 ty, Color color) {
    return {atlas, {tx * tile_size, ty * tile_size, tile_size, tile_size},
color};
}

void load_terrain_atlas() {
    auto atlas = textures::create("assets/kenney-1bitpack.png");
    INVARIANT(atlas, "failed to load assets");
    terrains::create("dirt", make_sprite_from_tilesheet(2,0,{206,197,183,255}));
}
*/

void generate_cave(Grid2<u32>& out, u32 wall, u32 floor, u32 birth, u32 survival, u32 range,
                   u32 iterations)
{
  DEBUG_ASSERT(out.size() > 0u);
  DEBUG_ASSERT(iterations > 0u);
  DEBUG_ASSERT(range > 0u);
  Grid2<u32> buffer(out.width(), out.height());

  const auto count_walls = [range, wall, &out](i64 x, i64 y) {
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

CaveGenerator::CaveGenerator(const Context ctx, u64 seed)
  : seed_{seed}
  , wall_{ctx.def.terrain.find("stone")}
  , floor_{ctx.def.terrain.find("dirt")}
{
  PRECONDITION(wall_ != nil);
  PRECONDITION(floor_ != nil);
}

void CaveGenerator::generate(u32 x, u32 y, Chunk& chunk)
{
  const auto hash = split_mix(seed_ ^ split_mix((u64{x} << 32) | y));
  Grid2<u32> cave(chunk_size, chunk_size);

  // uniformly random fill
  Xoshiro256ss                rng{hash};
  std::bernoulli_distribution coin{0.45f};
  for (auto& tile : cave) tile = coin(rng) ? wall_ : floor_;

  generate_cave(cave, wall_, floor_, 5, 6, 1, 1);
  generate_cave(cave, wall_, floor_, 3, 4, 2, 2);
  generate_cave(cave, wall_, floor_, 9, 10, 2, 1);

  for (auto [tile, terrain] : std::views::zip(chunk, cave)) { tile.terrain = terrain; }
}

// TODO: perlin noise
// procedural generation as a series of transformations


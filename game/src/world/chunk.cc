#include "catalog.hh"
#include "context.hh"
#include "engine/render/camera.hh"
#include "world/chunk.hh"
#include "world/terrain.hh"

void Chunk::render(ConstContext ctx, Camera const& camera, float tile_size) const
{
  auto catalog = access_catalog(ctx);
  for (auto coord : camera) {
    auto x    = u32(coord.x);
    auto y    = u32(coord.y);
    auto tile = get(x, y);
    if (!tile) continue;
    auto pixel = camera.view_coord_at({x, y}, tile_size);
    catalog[tile->terrain].sprite.draw(pixel.x, pixel.y, 1.0f);
  }
}

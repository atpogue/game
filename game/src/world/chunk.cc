#include "context.hh"
#include "engine/render/camera.hh"
#include "world/chunk.hh"

void Chunk::render(Context const ctx, Camera const& camera, float tile_size) const
{
  for (auto coord : camera) {
    u32  x = coord.x, y = coord.y;
    auto tile = get(x, y);
    if (!tile) continue;
    auto pixel = camera.view_coord_at({x, y}, tile_size);
    ctx.catalog[tile->terrain].sprite.draw(pixel.x, pixel.y, 1.0f);
  }
}

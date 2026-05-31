#include "engine/render/camera.hh"
#include "world/chunk.hh"
#include "world/terrain.hh"

void Chunk::render(const Camera &camera, float tile_size) const {
    for (auto coord : camera) {
        u32 x = coord.x, y = coord.y;
        auto tile = get(x, y);
        if (!tile) continue;
        auto pixel = camera.view_coord_at({x, y}) * tile_size;
        get_terrain(tile->terrain).sprite.draw(pixel.x, pixel.y, camera.zoom);
    }
}


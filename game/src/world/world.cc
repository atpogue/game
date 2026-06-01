#include "engine/core/error.hh"
#include "engine/render/camera.hh"
#include "world/terrain.hh"
#include "world/world.hh"

World::World(u32 width, u32 height,
             std::unique_ptr<ChunkGenerator> generator,
             std::unique_ptr<ChunkLoader> loader)
    : width_{width}, height_{height}
    , generator_(generator.release()), loader_(loader.release())
    , chunks_()
{
    ASSERT(width_ != 0u && height_ != 0u);
    PRECONDITION(generator_);
    PRECONDITION(loader_);
}

Tile &World::operator[](u32 x, u32 y) {
    ASSERT(has(x, y));
    return get_chunk(key_at(x, y))[x % chunk_size, y % chunk_size];
}

Tile *World::get(u32 x, u32 y) {
    if (!has(x, y)) return nullptr;
    return &get_chunk(key_at(x, y))[x % chunk_size, y % chunk_size];
}

const Tile *World::find(u32 x, u32 y) const {
    if (!has(x, y)) return nullptr;
    auto chunk = find_chunk(key_at(x, y));
    return chunk ? chunk->get(x % chunk_size, y % chunk_size) : nullptr;
}

const Chunk *World::find_chunk(u64 key) const {
    if (auto it = chunks_.find(key); it != chunks_.end())
        return &it->second;
    return nullptr;
}

Chunk &World::get_chunk(u64 key) {
    auto [it, is_new] = chunks_.try_emplace(key);
    auto &chunk = it->second;
    if (is_new) {
        u32 x, y; coord_of(key, x, y);
        generator_->generate(x, y, chunk);
        loader_->load(x, y, chunk);
        return chunk;
    }
    return chunk;
}

void World::render(const Camera &camera, float tile_size) const {
    for (auto coord : camera) {
        u32 x = coord.x, y = coord.y;
        wrap_around(x, y);
        auto tile = find(x, y);
        INVARIANT(tile, "tile not found despite wrap around");
        auto pixel = camera.view_coord_at({x, y}) * tile_size;
        get_terrain(tile->terrain).sprite.draw(pixel.x, pixel.y, camera.zoom);
    }
}


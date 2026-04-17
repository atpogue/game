#include "world/world.hh"

World::World(u32 width, u32 height)
    : width_{width}, height_{height}
    , chunks()
{
    assert(width_ != 0u && height_ != 0u);
}

Tile &World::operator[](u32 x, u32 y) {
    assert(has(x, y));
    return get_chunk(key_at(x, y)).tiles[x % chunk_size, y % chunk_size];
}

Tile *World::get(u32 x, u32 y) {
    if (!has(x, y)) return nullptr;
    return &get_chunk(key_at(x, y)).tiles[x % chunk_size, y % chunk_size];
}

const Tile *World::find(u32 x, u32 y) const {
    if (!has(x, y)) return nullptr;
    auto chunk = find_chunk(key_at(x, y));
    return chunk ? chunk->tiles.get(x % chunk_size, y % chunk_size) : nullptr;
}

const Chunk *World::find_chunk(u64 key) const {
    if (auto it = chunks.find(key); it != chunks.end())
        return &it->second;
    return nullptr;
}

Chunk &World::get_chunk(u64 key) {
    auto [it, is_new] = chunks.try_emplace(key);
    if (is_new) {
        load_chunk(key, it->second);
        return it->second;
    }
    return it->second;
}


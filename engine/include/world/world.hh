#pragma once
#include "world/chunk.hh"
#include "world/tile.hh"
#include <memory>
#include <unordered_map>

struct Camera;

struct World {

    World(u32 width, u32 height,
          std::unique_ptr<ChunkGenerator> generator,
          std::unique_ptr<ChunkLoader> loader);

    World(const World &other) = delete;
    World &operator=(const World &other) = delete;

    World(World &&other) = default; 
    World &operator=(World &&other) = default;

    constexpr u64 size() const { return u64{chunk_size} * width_ * height_; }

    constexpr bool has(u32 x, u32 y) const { return x  < width_ && y < height_; }

    void render(const Camera &camera, float tile_size) const;

    Tile &operator[](u32 x, u32 y);

    Tile *get(u32 x, u32 y);

    const Tile *find(u32 x, u32 y) const;

    constexpr void wrap_around(u32 &x, u32 &y) const {
        x %= (chunk_size * width_);
        y %= (chunk_size * height_);
    }

    constexpr auto begin() const { return chunks_.begin(); }
    constexpr auto begin()       { return chunks_.begin(); }

    constexpr auto end() const { return chunks_.end(); }
    constexpr auto end()       { return chunks_.end(); }

    constexpr u32 width() const { return width_; }
    constexpr u32 height() const { return height_; }

private:

    u32 width_, height_;
    std::unique_ptr<ChunkGenerator> generator_;
    std::unique_ptr<ChunkLoader> loader_;
    std::unordered_map<u64, Chunk> chunks_;

    constexpr u64 key_at(u32 x, u32 y) const {
        return x / chunk_size + (y / chunk_size) * width_;
    }

    constexpr void coord_of(u64 key, u32 &x, u32 &y) const {
        x = key % width_ * chunk_size;
        y = key / width_ * chunk_size;
    }

    const Chunk *find_chunk(u64 key) const;

    Chunk &get_chunk(u64 key);

};


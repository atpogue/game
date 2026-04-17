#pragma once
#include "core/types.hh"
#include <array>
#include <cassert>
#include <cstddef>
#include <vector>

// 3D grid stored on the heap whose size is unknown at comptile time
template <typename Type, u32 Width, u32 Height=0u, u32 Depth=0u>
struct Grid3 {

    Grid3(u32 w, u32 h, u32 d)
        : width_{w}, height_{h}, depth_{d}
        , data(size_t{width_} * height_ * depth_)
    {
        assert(width_ != 0u && height_ != 0u && depth_ != 0u);
    }

    Grid3(u32 w, u32 h, u32 d, const Type &value)
        : width_{w}, height_{h}, depth_{d}
        , data(size_t{width_} * height_ * depth_, value)
    {
        assert(width_ != 0u && height_ != 0u && depth_ != 0u);
    }

    Grid3(const Grid3 &other) = default;
    Grid3 &operator=(const Grid3 &other) = default;

    Grid3(Grid3 &&other) noexcept
        : width_{other.width_}, height_{other.height_}, depth_{other.depth_}
        , data(std::move(other.data))
    {
        other.width_ = 0u;
        other.height_ = 0u;
        other.depth_ = 0u;
    }

    Grid3 &operator=(Grid3 &&other) noexcept {
        if (this == &other) return;
        width_ = other.width_;
        height_ = other.height_;
        depth_ = other.depth_;
        data = std::move(other.data);
        other.width_ = 0u;
        other.height_ = 0u;
        other.depth_ = 0u;
        return *this;
    }

    constexpr size_t size() const {
        assert(data.size() == size_t{width_} * height_ * depth_);
        return data.size();
    }

    constexpr bool has(u32 x, u32 y, u32 z) const {
        return x < width_ && y < height_ && z < depth_;
    }

    constexpr const Type &operator[](u32 x, u32 y, u32 z) const { assert(has(x, y, z)); return data[index(x, y, z)]; }
    constexpr       Type &operator[](u32 x, u32 y, u32 z)       { assert(has(x, y, z)); return data[index(x, y, z)]; }

    constexpr const Type *get(u32 x, u32 y, u32 z) const { return has(x, y, z) ? &data[index(x, y, z)] : nullptr; }
    constexpr       Type *get(u32 x, u32 y, u32 z)       { return has(x, y, z) ? &data[index(x, y, z)] : nullptr; }

    constexpr auto begin() const { return data.begin(); }
    constexpr auto begin()       { return data.begin(); }

    constexpr auto end() const { return data.end(); }
    constexpr auto end()       { return data.end(); }

    constexpr u32 width() const { return width_; }
    constexpr u32 height() const { return height_; }
    constexpr u32 depth() const { return depth_; }

private:

    u32 width_, height_, depth_;
    std::vector<Type> data;

    constexpr size_t index(u32 x, u32 y, u32 z) const {
        size_t i = x + (y * size_t{width_}) + (z * size_t{width_} * height_);
        assert(i < data.size() && "invalid index");
        return i;
    }

};

// 3D grid whose size is known at comptile time
template <typename Type, u32 Width, u32 Height, u32 Depth>
requires (Width > 0u && Height > 0u && Depth > 0u)
struct Grid3<Type, Width, Height, Depth> {

    constexpr size_t size() const { return size_t{Width} * Height * Depth; }

    constexpr bool has(u32 x, u32 y, u32 z) const { return x < Width && y < Height && z < Depth; }

    constexpr const Type &operator[](u32 x, u32 y, u32 z) const { assert(has(x, y, z)); return data[index(x, y, z)]; }
    constexpr       Type &operator[](u32 x, u32 y, u32 z)       { assert(has(x, y, z)); return data[index(x, y, z)]; }

    constexpr const Type *get(u32 x, u32 y, u32 z) const { return has(x, y, z) ? &data[index(x, y, z)] : nullptr; }
    constexpr       Type *get(u32 x, u32 y, u32 z)       { return has(x, y, z) ? &data[index(x, y, z)] : nullptr; }

    constexpr auto begin() const { return data.begin(); }
    constexpr auto begin()       { return data.begin(); }

    constexpr auto end() const { return data.end(); }
    constexpr auto end()       { return data.end(); }

    constexpr u32 width() const { return Width; }
    constexpr u32 height() const { return Height; }
    constexpr u32 depth() const { return Depth; }

private:

    std::array<Type, size_t{Width} * Height * Depth> data;

    constexpr size_t index(u32 x, u32 y, u32 z) const {
        return x + (y * size_t{Width}) + (z * size_t{Width} * Height);
    }

};


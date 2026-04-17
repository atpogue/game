#pragma once
#include "core/types.hh"
#include <array>
#include <cassert>
#include <cstddef>
#include <vector>

// 2D grid stored on the heap whose size is unknown at comptile time
template <typename Type, u32 Width=0u, u32 Height=0u>
struct Grid2 {

    Grid2(u32 w, u32 h)
        : width_{w}, height_{h}, data(size_t{width_} * height_)
    {
        assert(width_ != 0u && height_ != 0u);
    }

    Grid2(u32 w, u32 h, const Type &value)
        : width_{w}, height_{h}, data(size_t{width_} * height_, value)
    {
        assert(width_ != 0u && height_ != 0u);
    }

    Grid2(const Grid2 &other) = default;
    Grid2 &operator=(const Grid2 &other) = default;

    Grid2(Grid2 &&other) noexcept
        : width_{other.width_}, height_{other.height_}
        , data(std::move(other.data))
    {
        other.width_ = 0u;
        other.height_ = 0u;
    }

    Grid2 &operator=(Grid2 &&other) noexcept {
        if (this == &other) return;
        width_ = other.width_;
        height_ = other.height_;
        data = std::move(other.data);
        other.width_ = 0u;
        other.height_ = 0u;
        return *this;
    }

    constexpr size_t size() const {
        assert(data.size() == size_t{width_} * height_);
        return data.size();
    }

    constexpr bool has(u32 x, u32 y) const {
        return x >= 0 && x < width_
            && y >= 0 && y < height_;
    }

    constexpr const Type &operator[](u32 x, u32 y) const { assert(has(x, y)); return data[index(x, y)]; }
    constexpr       Type &operator[](u32 x, u32 y)       { assert(has(x, y)); return data[index(x, y)]; }

    constexpr const Type *get(u32 x, u32 y) const { return has(x,y) ? &data[index(x,y)] : nullptr; }
    constexpr       Type *get(u32 x, u32 y)       { return has(x,y) ? &data[index(x,y)] : nullptr; }

    constexpr auto begin() const { return data.begin(); }
    constexpr auto begin()       { return data.begin(); }

    constexpr auto end() const { return data.end(); }
    constexpr auto end()       { return data.end(); }

    constexpr u32 width() const { return width_; }
    constexpr u32 height() const { return height_; }

private:

    u32 width_, height_;
    std::vector<Type> data;

    constexpr size_t index(u32 x, u32 y) const {
        size_t i = x + (y * size_t{width_});
        assert(i < data.size() && "invalid index");
        return i;
    }

};

// 2D grid whose size is known at comptile time
template <typename Type, u32 Width, u32 Height>
requires (Width > 0u && Height > 0u)
struct Grid2<Type, Width, Height> {

    constexpr size_t size() const { return size_t{Width} * Height; }

    constexpr bool has(u32 x, u32 y) const { return x < Width && y < Height; }

    constexpr const Type &operator[](u32 x, u32 y) const { assert(has(x, y)); return data[index(x, y)]; }
    constexpr       Type &operator[](u32 x, u32 y)       { assert(has(x, y)); return data[index(x, y)]; }

    constexpr const Type *get(u32 x, u32 y) const { return has(x, y) ? &data[index(x, y)] : nullptr; }
    constexpr       Type *get(u32 x, u32 y)       { return has(x, y) ? &data[index(x, y)] : nullptr; }

    constexpr auto begin() const { return data.begin(); }
    constexpr auto begin()       { return data.begin(); }

    constexpr auto end() const { return data.end(); }
    constexpr auto end()       { return data.end(); }

    constexpr u32 width() const { return Width; }
    constexpr u32 height() const { return Height; }

private:

    std::array<Type, size_t{Width} * Height> data;

    constexpr size_t index(u32 x, u32 y) const { return x + (y * size_t{Width}); }

};


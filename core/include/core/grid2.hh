#pragma once
#include "core/panic.hh"
#include "core/types.hh"
#include <array>
#include <cstddef>
#include <vector>

// 2D grid whose size is unknown at comptile time
template <typename Type, u32 Width = 0u, u32 Height = 0u>
struct Grid2
{
  Grid2(u32 w, u32 h) : width_{ w }, height_{ h }, data_(size_t{ width_ } * height_)
  {
    DEBUG_ASSERT(width_ != 0u && height_ != 0u, "constructed unusable grid");
  }

  Grid2(u32 w, u32 h, Type const& value)
    : width_{ w }, height_{ h }, data_(size_t{ width_ } * height_, value)
  {
    DEBUG_ASSERT(width_ != 0u && height_ != 0u, "constructed unusable grid");
  }

  Grid2(Grid2&& other) noexcept
    : width_{ other.width_ }, height_{ other.height_ }, data_(std::move(other.data_))
  {
    other.width_  = 0u;
    other.height_ = 0u;
  }

  Grid2& operator=(Grid2&& other) noexcept
  {
    if (this == &other) return *this;
    width_        = other.width_;
    height_       = other.height_;
    data_         = std::move(other.data_);
    other.width_  = 0u;
    other.height_ = 0u;
    return *this;
  }

  constexpr size_t size() const
  {
    INVARIANT(
      data_.size() == size_t{ width_ } * height_, "size of data array must match grid dimensions");
    return data_.size();
  }

  constexpr bool has(u32 x, u32 y) const { return x < width_ && y < height_; }

  constexpr Type const& operator[](u32 x, u32 y) const
  {
    DEBUG_ASSERT(has(x, y));
    return data_[index(x, y)];
  }

  constexpr Type& operator[](u32 x, u32 y)
  {
    DEBUG_ASSERT(has(x, y));
    return data_[index(x, y)];
  }

  constexpr Type const* get(u32 x, u32 y) const
  {
    return has(x, y) ? &data_[index(x, y)] : nullptr;
  }

  constexpr Type* get(u32 x, u32 y) { return has(x, y) ? &data_[index(x, y)] : nullptr; }

  constexpr auto begin() const { return data_.begin(); }

  constexpr auto begin() { return data_.begin(); }

  constexpr auto end() const { return data_.end(); }

  constexpr auto end() { return data_.end(); }

  constexpr u32 width() const { return width_; }

  constexpr u32 height() const { return height_; }

  Grid2<Type> copy() const { return *this; }

  template <u32 W, u32 H>
  bool insert(u32 x, u32 y, Grid2<Type, W, H> const& src)
  {
    if (x + src.width() > width_ || y + src.height() > height_) return false;
    for (u32 j = 0u; j < src.height(); ++j)
      for (u32 i = 0u; i < src.width(); ++i) (*this)[x + i, y + j] = src[i, j];
    return true;
  }

private:

  u32               width_, height_;
  std::vector<Type> data_;
  Grid2(Grid2 const& other)            = default;
  Grid2& operator=(Grid2 const& other) = default;

  constexpr size_t index(u32 x, u32 y) const
  {
    size_t i = x + (y * size_t{ width_ });
    INVARIANT(i < data_.size(), "translated coordinates to invalid index");
    return i;
  }
};

// 2D grid whose size is known at compile time
template <typename Type, u32 Width, u32 Height>
requires (Width > 0u && Height > 0u)
struct Grid2<Type, Width, Height>
{
  constexpr size_t size() const { return size_t{ Width } * Height; }

  constexpr bool has(u32 x, u32 y) const { return x < Width && y < Height; }

  constexpr Type const& operator[](u32 x, u32 y) const
  {
    DEBUG_ASSERT(has(x, y));
    return data_[index(x, y)];
  }

  constexpr Type& operator[](u32 x, u32 y)
  {
    DEBUG_ASSERT(has(x, y));
    return data_[index(x, y)];
  }

  constexpr Type const* get(u32 x, u32 y) const
  {
    return has(x, y) ? &data_[index(x, y)] : nullptr;
  }

  constexpr Type* get(u32 x, u32 y) { return has(x, y) ? &data_[index(x, y)] : nullptr; }

  constexpr auto begin() const { return data_.begin(); }

  constexpr auto begin() { return data_.begin(); }

  constexpr auto end() const { return data_.end(); }

  constexpr auto end() { return data_.end(); }

  constexpr u32 width() const { return Width; }

  constexpr u32 height() const { return Height; }

private:

  std::array<Type, size_t{ Width } * Height> data_;

  constexpr size_t index(u32 x, u32 y) const { return x + (y * size_t{ Width }); }
};

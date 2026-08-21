#pragma once
#include "core/types.hh"
#include <glm/common.hpp>
#include <glm/ext/vector_float2.hpp>

struct Rectangle
{
  glm::vec2 origin, extent;

  constexpr bool contains(glm::vec2 coord) const
  {
    return coord.x >= origin.x && coord.y >= origin.y && coord.y < origin.y + extent.y
        && coord.x < origin.x + extent.x;
  }

  [[nodiscard]] constexpr glm::vec2 min() const noexcept { return origin; }

  [[nodiscard]] constexpr glm::vec2 max() const noexcept { return origin + extent; }

  [[nodiscard]] constexpr f32 x() const noexcept { return origin.x; }

  [[nodiscard]] constexpr f32 y() const noexcept { return origin.y; }

  [[nodiscard]] constexpr f32 width() const noexcept { return extent.x; }

  [[nodiscard]] constexpr f32 height() const noexcept { return extent.y; }

  [[nodiscard]] constexpr f32 area() const noexcept { return extent.x * extent.y; }

  [[nodiscard]] Rectangle clip(Rectangle bounds) const
  {
    auto const lo = glm::clamp(bounds.min(), min(), max());
    auto const hi = glm::clamp(bounds.max(), lo, max());
    return Rectangle(lo, hi - lo);
  }
};


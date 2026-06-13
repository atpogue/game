#pragma once
#include "engine/core/types.hh"

#include <compare>

// Used to represent a reference that can become stale (referring a deleted
// element) similar to the role of a weak pointer.
template <typename Tag = void> struct Handle
{

  u32 index      = nil;
  u32 generation = nil;

  [[nodiscard]] static constexpr Handle<Tag> null() noexcept { return {nil, nil}; }

  [[nodiscard]] std::strong_ordering operator<=>(const Handle<Tag>&) const noexcept = default;

  template <typename T = void> [[nodiscard]] constexpr Handle<T> with_tag() const noexcept
  {
    return {index, generation};
  }

  [[nodiscard]] constexpr explicit operator bool() const noexcept
  {
    return *this != Handle<Tag>::null();
  }
};


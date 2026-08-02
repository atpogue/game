#pragma once
#include <compare>
#include <cstdint>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

constexpr u32 nil = UINT32_MAX;

enum AccessPolicy : bool { Read = false, Write = true };

// A stable, type-tagged index into an append-only container (i.e. `Catalog`).
// The tag prevents IDs representing different resources being silently interchanged.
template <typename Tag>
struct Token
{
  // I would call this a Key but that's already used as a template parameter for containers.

  u32 value = nil;

  [[nodiscard]] static consteval Token null() noexcept { return {}; }

  [[nodiscard]] constexpr std::strong_ordering operator<=>(Token const&) const noexcept = default;

  [[nodiscard]] constexpr explicit operator bool() const noexcept { return value != nil; }
};

// A type-tagged identifier to a recyclable index that may become stale (i.e. refer to a deleted
// resource) similar to the role of a weak pointer.
template <typename Tag = void>
struct Handle
{
  u32 index      = nil;
  u32 generation = nil;

  [[nodiscard]] static consteval Handle<Tag> null() noexcept { return {nil, nil}; }

  [[nodiscard]] constexpr std::strong_ordering operator<=>(Handle<Tag> const&) const noexcept
    = default;

  [[nodiscard]] constexpr explicit operator bool() const noexcept
  {
    return index != nil && generation != nil;
  }
};

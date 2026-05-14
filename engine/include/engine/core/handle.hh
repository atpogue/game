#pragma once
#include "engine/core/types.hh"
#include <compare>

// Used to represent a reference that can become stale (referring a deleted element)
// similar to the role of a weak pointer.
template <typename Tag>
struct Handle {

    u32 index      = nil;
    u32 generation = nil;

    [[nodiscard]] static constexpr Handle<Tag> null() { return {nil, nil}; }

    std::strong_ordering operator<=>(const Handle<Tag> &) const = default;

    constexpr operator Handle<void>() const { return {index, generation}; }

    template <typename T>
    constexpr explicit operator Handle<T>() const { return {index, generation}; }

    constexpr explicit operator bool() const { return *this != Handle<Tag>::null(); }

};


#pragma once
#include "core/types.hh"

constexpr auto handle_index_max      = UINT32_MAX;
constexpr auto handle_generation_max = UINT32_MAX;

template <typename Tag>
struct Handle {
    u32 index      = handle_index_max;
    u32 generation = handle_generation_max;
    auto operator<=>(const Handle<Tag> &) const = default;
    constexpr explicit operator bool() const { return *this != Handle<Tag>::null(); }
    static constexpr Handle<Tag> null() { return {handle_index_max, handle_generation_max}; }
};


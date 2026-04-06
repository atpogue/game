#pragma once
#include <cstdint>

using Id    = uint64_t;
using Index = uint32_t;

constexpr auto handle_index_max      = UINT32_MAX;
constexpr auto handle_generation_max = UINT32_MAX;

template <typename Tag>
struct Handle {
    Index index      = handle_index_max;
    Index generation = handle_generation_max;
    bool operator==(const Handle<Tag> &) const = default;
    static constexpr Handle<Tag> null() { return {handle_index_max, handle_generation_max}; }
};


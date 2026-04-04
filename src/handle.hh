#pragma once
#include <cstdint>

constexpr auto handle_index_max      = UINT32_MAX;
constexpr auto handle_generation_max = UINT32_MAX;

template <typename Tag>
struct Handle {
    uint32_t index      = handle_index_max;
    uint32_t generation = handle_generation_max;
    static constexpr Handle<Tag> null() { return {handle_index_max, handle_generation_max}; }
    bool operator==(const Handle<Tag> &) const = default;
};


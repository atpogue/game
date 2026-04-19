#pragma once
#include "core/types.hh"
#include <concepts>
#include <functional>

template <typename Type>
struct TransparentHash {
    using is_transparent = void;

    template <typename Value>
    requires std::convertible_to<Value, Type>
    size_t operator()(Value &&value) const noexcept {
        return std::hash<std::remove_cvref_t<Value>>{}(std::forward<Value>(value));
    }
};

// The output function of SplitMix64 PRNG used as a bit mixer
constexpr u64 split_mix(u64 z) {
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

// Based on boost::hash_combine
constexpr u64 hash_combine(u64 seed, u64 value) {
    return seed ^ (split_mix(value) + 0x9e3779b97f4a7c15ULL + (seed << 12) + (seed >> 4));
}


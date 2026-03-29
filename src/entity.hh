#pragma once
#include <array>
#include <bitset>
#include <cstdint>

using Entity = uint32_t;
constexpr Entity NULL_ENTITY = 0;

enum Component : uint8_t {
    COMPONENT_MOVE,
    COMPONENT_COUNT
};

using Signature = std::bitset<COMPONENT_COUNT>;
using Index = std::array<size_t, COMPONENT_COUNT>;


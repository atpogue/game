#pragma once
#include "render/graphics.hh"
#include <string>

struct Terrain {
    using Id = uint16_t;

    enum class Flag : uint8_t {
        None    = 0,
        Collide = 1 << 0,
    };

    std::string name;
    Sprite sprite;
    Flag flag = Flag::None;
};

constexpr Terrain::Id terrain_max = UINT16_MAX;

namespace terrain {

    const Terrain &get(Terrain::Id id);
    Terrain::Id create(std::string name, Sprite sprite, Terrain::Flag flag);

} // namspace terrian


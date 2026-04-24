#pragma once
#include "render/sprites.hh"
#include <optional>
#include <string_view>

struct Terrain {
    using Id = u32;

    enum class Flag : u8 {
        None    = 0,
        Collide = 1 << 0,
    };

    std::string name;
    Sprite sprite;
    Flag flag = Flag::None;
};

namespace terrains {

    std::optional<Terrain::Id> find(std::string_view name);
    const Terrain &get(Terrain::Id id); // assumes the ID is valid
    std::optional<Terrain::Id> create(std::string_view name, Sprite sprite, Terrain::Flag flag = Terrain::Flag::None);

} // namspace terrians



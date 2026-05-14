#pragma once
#include "engine/render/textures.hh"
#include <string>
#include <string_view>

struct Terrain {
    std::string name;
    Sprite sprite;

    enum class Flag : u8 {
        None    = 0,
        Collide = 1 << 0,
    } flag = Flag::None;
};

u32 find_terrain(std::string_view name);
const Terrain &get_terrain(u32 id); // assumes the ID is valid
u32 create_terrain(std::string_view name, Sprite sprite, Terrain::Flag flag = Terrain::Flag::None);


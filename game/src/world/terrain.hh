#pragma once
#include "sprite.hh"
#include "engine/core/catalog.hh"
#include <string>

struct Terrain {
    std::string name;
    Sprite sprite;
    enum class Flag : u8 {
        None    = 0,
        Collide = 1 << 0,
    } flag = Flag::None;
};

Catalog<Terrain> load_terrain();


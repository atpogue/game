#pragma once
#include "engine/entity.hh"
#include <vector>

struct Tile {
    u32 terrain;
    std::vector<Entity> occupants;
};


#pragma once
#include "engine/world/terrain.hh"
#include <vector>

struct Tile {
    Terrain::Id terrain;
    std::vector<Entity> occupants;
};


#pragma once
#include "world/terrain.hh"
#include <vector>

struct Tile {
    Terrain::Id terrain;
    std::vector<Entity> occupants;
};


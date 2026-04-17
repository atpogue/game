#include "world/terrain.hh"
#include <cassert>
#include <vector>

namespace {
    std::vector<Terrain> data;
}

const Terrain &terrain::get(Terrain::Id id) {
    assert(id < data.size() && "invalid terrain id");
    return data[id];
}

Terrain::Id terrain::create(std::string name, Sprite sprite, Terrain::Flag flag) {
    data.emplace_back(name, sprite, flag);
    return data.size() - 1u;
}


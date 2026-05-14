#include "engine/world/terrain.hh"
#include "engine/core/hash.hh"
#include <cassert>
#include <vector>
#include <limits>
#include <unordered_map>

namespace {
    std::vector<Terrain> data;
    // transparent hash allows lookup with string views
    std::unordered_map<std::string, u32, TransparentHash<std::string_view>, std::equal_to<>> lookup;
}

const Terrain &get_terrain(u32 id) {
    assert(id < data.size() && "invalid terrain id");
    return data[id];
}

u32 find_terrain(std::string_view name) {
    if (auto it = lookup.find(name); it != lookup.end()) return it->second;
    return nil;
}

u32 create_terrain(std::string_view name, Sprite sprite, Terrain::Flag flag) {
    if (data.size() > std::numeric_limits<u32>::max()) return nil;
    // check validity of sprite atlas
    u32 id = data.size();
    if (auto [_, is_new] = lookup.emplace(name, id); !is_new) return nil;
    data.emplace_back(std::string(name), sprite, flag);
    return id;
}


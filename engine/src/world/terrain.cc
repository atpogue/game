#include "engine/world/terrain.hh"
#include "engine/core/hash.hh"
#include <cassert>
#include <vector>
#include <limits>
#include <unordered_map>

namespace {
    std::vector<Terrain> data;
    // transparent hash allows lookup with string views
    std::unordered_map<std::string, Terrain::Id, TransparentHash<std::string_view>, std::equal_to<>> lookup;
}

const Terrain &terrains::get(Terrain::Id id) {
    assert(id < data.size() && "invalid terrain id");
    return data[id];
}

std::optional<Terrain::Id> terrains::find(std::string_view name) {
    if (auto it = lookup.find(name); it != lookup.end())
        return std::make_optional(it->second);
    return {};
}

std::optional<Terrain::Id> terrains::create(std::string_view name, Sprite sprite, Terrain::Flag flag) {
    if (data.size() > std::numeric_limits<Terrain::Id>::max()) return {};
    // check validity of sprite atlas
    Terrain::Id id = data.size();
    if (auto [_, is_new] = lookup.emplace(name, id); !is_new) return {};
    data.emplace_back(std::string(name), sprite, flag);
    return std::make_optional(id);
}


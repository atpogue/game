#include "engine/core/catalog.hh"
#include "engine/render/textures.hh"
#include "world/terrain.hh"

namespace { /////////////////////////////////////////////////////////
    Catalog<Terrain> catalog;
} ///////////////////////////////////////////////////////////////////

// Assumptions: ID is valid.
const Terrain &get_terrain(u32 id) { PRECONDITION(id != nil); return catalog[id]; }

u32 find_terrain(std::string_view name) { return catalog.find(name); }

bool load_terrain() {
    auto texture = create_texture("assets/kenney-1bitpack.png");
    // throw exception here??
    PANIC(texture, "failed to load texture from PNG file");
    return nil != catalog.emplace("grass-1",       make_sprite_1x1(texture, 5, 0, Color{59,216,114,255}))
        && nil != catalog.emplace("grass-2",       make_sprite_1x1(texture, 6, 0, Color{59,216,114,255}))
        && nil != catalog.emplace("grass-3",       make_sprite_1x1(texture, 7, 0, Color{59,216,114,255}))
        && nil != catalog.emplace("grass-tall",    make_sprite_1x1(texture, 0, 2, Color{59,216,114,255}))
        && nil != catalog.emplace("dirt",          make_sprite_1x1(texture, 2, 0, Color{121,70,75,255}))
        && nil != catalog.emplace("rocks",         make_sprite_1x1(texture, 2, 0, Color{206,197,183,255}));
}


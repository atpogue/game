#include "engine/render/textures.hh"
#include "world/terrain.hh"

Catalog<Terrain> load_terrain() {
    auto texture = create_texture("assets/kenney-1bitpack.png");
    // throw an io exception here??
    ASSERT(texture, "failed to load texture from PNG file");
    Catalog<Terrain> catalog;
    catalog.emplace("grass-1",       make_sprite_1x1(texture, 5, 0, Color{59,216,114,255}));
    catalog.emplace("grass-2",       make_sprite_1x1(texture, 6, 0, Color{59,216,114,255}));
    catalog.emplace("grass-3",       make_sprite_1x1(texture, 7, 0, Color{59,216,114,255}));
    catalog.emplace("grass-tall",    make_sprite_1x1(texture, 0, 2, Color{59,216,114,255}));
    catalog.emplace("dirt",          make_sprite_1x1(texture, 2, 0, Color{121,70,75,255}));
    catalog.emplace("rocks",         make_sprite_1x1(texture, 2, 0, Color{206,197,183,255}));
    return catalog;
}


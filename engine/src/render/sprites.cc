#include "render/internal.hh"
#include "engine/render/sprites.hh"
#include "engine/render/textures.hh"
#include "engine/core/sparse-map.hh"

namespace {
    SparseMap<Sprite> components;
}

void Sprite::draw(float x, float y, float scale) const {
    auto renderer = get_renderer();
    SDL_Texture *texture = get_texture(atlas);
    assert(texture && "sprite atlas does not map to a texture");
    Rectangle to{x, y, source.w * scale, source.h * scale};
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_RenderTexture(renderer, texture, &source, &to);
}

Sprite *sprites::get(Entity e) {
    return ledger::status(e) ? components.get(e.index) : nullptr;
}

Sprite *sprites::create(Entity e, Handle<Texture> atlas, Rectangle source, Color color) {
    if (!ledger::status(e) || !get_texture(atlas)) return nullptr;
    ledger::sign(e, Flag::Sprite, true);
    return &components.emplace(e.index, atlas, source, color);
}

void sprites::destroy(Entity e) {
    components.erase(e.index);
}


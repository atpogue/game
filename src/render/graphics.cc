#include "render/graphics.hh"
#include "core/slot-map.hh"
#include "core/sparse-map.hh"
#include <map>
#include <string>
#include <string_view>
#include <SDL3/SDL_log.h>

namespace {
    SDL_Renderer *renderer = nullptr; // non-owning pointer
    SlotMap<Texture> textures; 
    SparseMap<Sprite> sprites;
    std::map<std::string, Handle<Texture>, std::less<>> texture_paths;
}

bool graphics::init(SDL_Renderer *r) {
    renderer = r;
    return renderer != nullptr;
}

void graphics::quit() {
    textures.clear();
    sprites.clear();
    texture_paths.clear();
}

Handle<Texture> graphics::create_texture(std::string_view path) {
    if (auto it = texture_paths.find(path); it != texture_paths.end())
        return it->second;
    SDL_Surface *surface = SDL_LoadPNG(path.data());
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return Handle<Texture>::null();
    }
    auto handle = graphics::create_texture(surface);
    SDL_DestroySurface(surface);
    texture_paths.emplace(path, handle);
    return handle;
}

Handle<Texture> graphics::create_texture(SDL_Surface *surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return Handle<Texture>::null();
    }
    return textures.emplace(texture, SDL_DestroyTexture);
}

SDL_Texture *graphics::texture(Handle<Texture> handle) {
    Texture *texture = textures.get(handle);
    return texture ? texture->get() : nullptr;
}

void graphics::destroy_texture(Handle<Texture> handle) {
    textures.erase(handle);
}

Sprite *graphics::sprite(Entity e) {
    return ledger::status(e) ? sprites.get(e.index) : nullptr;
}

Sprite *graphics::create_sprite(Entity e, Handle<Texture> atlas, SDL_FRect source) {
    if (!ledger::status(e) || !textures.status(atlas)) return nullptr;
    ledger::sign(e, Flag::Sprite, true);
    return &sprites.emplace(e.index, atlas, source);
}

void graphics::destroy_sprite(Entity e) {
    sprites.erase(e.index);
}

void graphics::render(Entity entity, const SDL_FRect &destination) {
    auto s = graphics::sprite(entity);
    auto t = graphics::texture(s->atlas);
    if (s) SDL_RenderTexture(renderer, t, &s->source, &destination);
}


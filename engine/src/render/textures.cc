#include "render/internal.hh"
#include "engine/render/textures.hh"
#include "engine/core/slot-map.hh"
#include <map>
#include <string>
#include <string_view>
#include <SDL3/SDL_log.h>

struct Texture {
    explicit Texture(SDL_Texture *t) : data{t} {}
    ~Texture() { SDL_DestroyTexture(data); }
    SDL_Texture *get() const { return data; }
private:
    SDL_Texture *data;
};

namespace {
    SlotMap<Texture> components; 
    std::map<std::string, Handle<Texture>, std::less<>> paths;
}

void textures::quit() {
    components.clear();
    paths.clear();
}

Handle<Texture> textures::create(std::string_view path) {
    if (auto it = paths.find(path); it != paths.end())
        return it->second;
    SDL_Surface *surface = SDL_LoadPNG(path.data());
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return Handle<Texture>::null();
    }
    auto handle = textures::create(surface);
    SDL_DestroySurface(surface);
    paths.emplace(path, handle);
    return handle;
}

Handle<Texture> textures::create(SDL_Surface *surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(get_renderer(), surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return Handle<Texture>::null();
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return components.emplace(texture);
}

SDL_Texture *get_texture(Handle<Texture> handle) {
    Texture *texture = components.get(handle);
    return texture ? texture->get() : nullptr;
}

void textures::destroy(Handle<Texture> handle) {
    components.erase(handle);
}


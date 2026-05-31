#include "engine/render/textures.hh"
#include "engine/core/invariant.hh"
#include "engine/core/slot-map.hh"
#include "render/internal.hh"
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

namespace { /////////////////////////////////////////////////////
    
    SlotMap<Texture> textures; 
    std::map<std::string, Handle<Texture>, std::less<>> paths;

} ///////////////////////////////////////////////////////////////

// needs happen before SDL quit
void destroy_all_textures() { textures.clear(); }

SDL_Texture *get_texture(Handle<Texture> handle) {
    Texture *texture = textures.get(handle);
    return texture ? texture->get() : nullptr;
}

/// PUBLIC API //////////////////////////////////////////////////

Handle<Texture> create_texture(std::string_view path) {
    if (auto it = paths.find(path); it != paths.end())
        return it->second;
    SDL_Surface *surface = SDL_LoadPNG(path.data());
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return Handle<Texture>::null();
    }
    auto handle = create_texture(surface);
    SDL_DestroySurface(surface);
    paths.emplace(path, handle);
    return handle;
}

Handle<Texture> create_texture(SDL_Surface *surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(get_renderer(), surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return Handle<Texture>::null();
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    auto handle = textures.emplace(texture);
    INVARIANT(handle, "texture limit reached");
    return handle;
}

void destroy_texture(Handle<Texture> handle) {
    textures.erase(handle);
}

void draw_texture(Handle<Texture> handle, Rectangle source, Rectangle dest, Color tint) {
    auto renderer = get_renderer();
    SDL_Texture *texture = get_texture(handle);
    assert(texture && "texture doesn't exist");
    SDL_SetTextureColorMod(texture, tint.r, tint.g, tint.b);
    SDL_RenderTexture(renderer, texture, &source, &dest);
}


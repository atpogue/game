#pragma once
#include "core/handle.hh"
#include "ecs/ledger.hh"
#include <memory>
#include <SDL3/SDL_render.h>

using Texture = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using Surface = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;

struct Sprite {
    Handle<Texture> atlas;
    SDL_FRect source;
};

namespace graphics {

    // return non-owning pointer for rendering
    SDL_Texture *get_texture(Handle<Texture> handle);
    Handle<Texture> create_texture(std::string_view path);
    Handle<Texture> create_texture(SDL_Surface *surface);
    void destroy_texture(Handle<Texture> handle);

    Sprite *get_sprite(Entity e);
    Sprite *create_sprite(Entity e, Handle<Texture> atlas, SDL_FRect source);
    void destroy_sprite(Entity e);

    bool init(SDL_Renderer *r);
    void quit();
    void render(Entity entity, const SDL_FRect &destination);

} // namespace graphics


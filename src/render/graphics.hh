#pragma once
#include "core/sdlxx.hh"
#include "ecs/ledger.hh"

struct Sprite {
    Handle<Texture> atlas;
    Rectangle source;
    Color color;
};

namespace graphics {

    // return non-owning pointer for rendering
    SDL_Texture *get_texture(Handle<Texture> handle);
    Handle<Texture> create_texture(std::string_view path);
    Handle<Texture> create_texture(SDL_Surface *surface);
    void destroy_texture(Handle<Texture> handle);

    Sprite *get_sprite(Entity e);
    Sprite *create_sprite(Entity e, Handle<Texture> atlas, Rectangle source, Color color);
    void destroy_sprite(Entity e);

    bool init(SDL_Renderer *r);
    void quit();
    void render(Entity entity, const Rectangle &destination);

} // namespace graphics


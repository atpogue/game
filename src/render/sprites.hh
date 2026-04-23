#pragma once
#include "ecs/ledger.hh"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>

struct Texture;

using Color = SDL_Color;
using Rectangle = SDL_FRect;

struct Sprite {

    Handle<Texture> atlas;
    Rectangle source;
    Color color;

    // assumes the sprite is valid
    void draw(float x, float y, float scale=1.f) const;

};

namespace sprites {

    Sprite *get(Entity e);
    Sprite *create(Entity e, Handle<Texture> atlas, Rectangle source, Color color);
    void destroy(Entity e);

} // namespace sprites


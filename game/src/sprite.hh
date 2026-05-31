#pragma once
#include "engine/render/textures.hh"

struct Sprite {

    Handle<Texture> atlas;
    Rectangle source;
    Color color;

    // assumes the sprite is valid
    void draw(float x, float y, float scale=1.f) const;

};

Sprite make_sprite_1x1(Handle<Texture> atlas, f32 x, f32 y, Color color);


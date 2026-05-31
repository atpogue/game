#include "sprite.hh"
#include "world/tile.hh"

Sprite make_sprite_1x1(Handle<Texture> atlas, f32 x, f32 y, Color color) {
    return {atlas, {x * tile_size, y * tile_size, tile_size, tile_size}, color};
}

void Sprite::draw(float x, float y, float scale) const {
    draw_texture(atlas, source, {x, y, source.w * scale, source.h * scale}, color);
}


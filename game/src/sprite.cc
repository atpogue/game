#include "engine/core/defer.hh"
#include "engine/core/lua.hh"
#include "world/tile.hh"
#include "sprite.hh"

Sprite make_sprite_1x1(Handle<Texture> atlas, f32 x, f32 y, Color color) {
    return {atlas, {x * tile_size, y * tile_size, tile_size, tile_size}, color};
}

void Sprite::draw(float x, float y, float scale) const {
    draw_texture(atlas, source, {x, y, source.w * scale, source.h * scale}, color);
}

// sprite = { atlas = "path", x = N, y = N, color = N }
Result<Sprite> lua::try_get_sprite(lua_State *L, int idx, std::string_view field) {
    auto sprite = try_push_field(L, LUA_TTABLE, idx, field);
    if (!sprite) return std::unexpected(sprite.error());
    DEFER(lua_pop(L, 1));

    auto atlas = try_get_string(L, *sprite, "atlas");
    if (!atlas) return std::unexpected(atlas.error());

    auto x = try_get_integer(L, *sprite, "x");
    if (!x) return std::unexpected(x.error());

    auto y = try_get_integer(L, *sprite, "y");
    if (!y) return std::unexpected(y.error());

    auto color = try_get_integer(L, *sprite, "color");
    if (!color) return std::unexpected(color.error());

    return make_sprite_1x1(create_texture(*atlas), *x, *y, hex_color(*color));
}


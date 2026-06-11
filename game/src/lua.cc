#include "lua.hh"
#include "sprite.hh"
#include "engine/core/defer.hh"

using namespace lua;

// sprite = { atlas = "path", x = N, y = N, color = N }
Result<Sprite> lua::parse_sprite_table(lua_State *L, int table, std::string_view field) {
    auto sprite = try_push_field(L, LUA_TTABLE, table, field);
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


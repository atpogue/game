#pragma once
#include "engine/core/lua.hh"

struct Sprite;

namespace lua {

    Result<Sprite> parse_sprite_table(lua_State *L, int table, std::string_view field);

}

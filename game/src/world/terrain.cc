#include "core/basic-catalog.hh"
#include "core/defer.hh"
#include "core/lua.hh"
#include "sprite.hh"
#include "types.hh"
#include "world/terrain.hh"
#include <lua.hpp>

// terrain "name" { sprite = {} }
static int parse_terrain_table(lua_State* L)
{
  INVARIANT(lua_islightuserdata(L, lua_upvalueindex(1)));
  INVARIANT(lua_isstring(L, lua_upvalueindex(2)));
  auto catalog = static_cast<Catalog*>(lua_touserdata(L, lua_upvalueindex(1)));
  auto name    = lua_tostring(L, lua_upvalueindex(2));
  do {
    // arg 1: definition table
    if (!lua_istable(L, 1)) {
      lua::push_fstring(
        L, "terrain '{}': expected table, found {}", name, lua_typename(L, lua_type(L, 1))
      );
      break;
    }

    int  terrain = lua_gettop(L);
    auto sprite  = lua::try_get_sprite(L, terrain, "sprite");
    if (!sprite) {
      lua::push_string(L, sprite.error().msg);
      break;
    }

    DEFER(lua_pop(L, 1));
    catalog->emplace<Terrain>(name, *sprite);
    return 0;
  } while (false);
  // this will unwind the stack without calling C++ destructors
  return lua_error(L);
}

static int build_terrain(lua_State* L)
{
  INVARIANT(lua_islightuserdata(L, lua_upvalueindex(1)));
  // arg 1: name string
  if (!lua_isstring(L, 1)) {
    lua_pushstring(
      L, std::format("terrain: expected string, found {}", lua_typename(L, lua_type(L, 1))).data()
    );
    return lua_error(L);
  }
  lua_pushvalue(L, lua_upvalueindex(1));
  lua_pushvalue(L, 1); // name
  lua_pushcclosure(L, parse_terrain_table, 2);
  return 1;
}

void lua::add_terrain_builder(lua_State* L, Catalog& catalog)
{
  lua_pushlightuserdata(L, &catalog);
  lua_pushcclosure(L, build_terrain, 1);
  lua_setglobal(L, "terrain");
}

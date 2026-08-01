#include "core/basic-catalog.hh"
#include "core/lua.hh"
#include "data.hh"
#include "world/terrain.hh"
#include <SDL3/SDL_log.h>
#include <lua.hpp>

bool load_content(Catalog& catalog, std::string_view path)
{
  lua_State* L = luaL_newstate();
  ASSERT(L != nullptr, "failed to create lua state");
  lua::add_terrain_builder(L, catalog);
  auto result = lua::do_file(L, path);
  if (!result) {
    SDL_LogError(
      SDL_LOG_CATEGORY_ERROR, "Error in %s:\n\t %s", path.data(), // NOLINT
      result.error().msg.data()
    );
    return false;
  }
  INVARIANT(lua_gettop(L) == 0 && "unprocessed items still on the lua stack");
  lua_close(L);
  return true;
}

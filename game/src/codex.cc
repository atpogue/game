#include "codex.hh"

#include "world/terrain.hh"

#include "engine/core/lua.hh"

#include <SDL3/SDL_log.h>
#include <lua.hpp>

bool load_content(Codex& codex, std::string_view path)
{
  lua_State* L = luaL_newstate();
  ASSERT(L != nullptr, "failed to create lua state");

  lua::add_terrain_builder(L, codex);

  auto result = lua::do_file(L, path);
  if (!result) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error in %s:\n\t %s", path.data(),
                 result.error().msg.data());
    return false;
  }

  INVARIANT(lua_gettop(L) == 0 && "unprocessed items still on the lua stack");
  lua_close(L);
  return true;
}


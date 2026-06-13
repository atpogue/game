#include "core/lua.hh"
#include "core/defer.hh"
#include "core/panic.hh"
#include <format>
#include <lauxlib.h>
#include <lualib.h>
using namespace lua;

static int handle_error(lua_State* L)
{
  luaL_traceback(L, L, lua_tostring(L, -1), 1);
  return 1;
}

Error lua::make_content_error(lua_State* L, int found, int expected)
{
  return {std::format("expected {}, found {}", lua_typename(L, expected), lua_typename(L, found))};
}

void lua::push_string(lua_State* L, std::string_view value)
{
  lua_pushlstring(L, value.data(), value.size()); // string may not be null-terminated
}

bool lua::pop_boolean(lua_State* L)
{
  bool out = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return out;
}

std::string lua::pop_string(lua_State* L)
{
  std::string out = lua_tostring(L, -1);
  lua_pop(L, 1);
  return out;
}

lua_Integer lua::pop_integer(lua_State* L)
{
  auto out = lua_tointeger(L, -1);
  lua_pop(L, 1);
  return out;
}

lua_Number lua::pop_number(lua_State* L)
{
  auto out = lua_tonumber(L, -1);
  lua_pop(L, 1);
  return out;
}

Result<int> lua::try_push_field(lua_State* L, int type, int table, unsigned field)
{
  PRECONDITION(L);
  PRECONDITION(table != 0);
  PRECONDITION(lua_istable(L, table));
  PRECONDITION(field > 0u);
  int found = lua_geti(L, table, field);
  if (found == type) return lua_gettop(L);
  lua_pop(L, -1);
  return std::unexpected(lua::make_content_error(L, found, type));
}

Result<int> lua::try_push_field(lua_State* L, int type, int table, std::string_view field)
{
  PRECONDITION(L);
  PRECONDITION(table != 0);
  PRECONDITION(lua_istable(L, table));
  table = lua_absindex(L, table);
  push_string(L, field);
  int found = lua_gettable(L, table);
  if (found == type) return lua_gettop(L);
  lua_pop(L, 1);
  return std::unexpected(lua::make_content_error(L, found, type));
}

Result<std::string> lua::try_get_string(lua_State* L, int table, std::string_view field)
{
  auto idx = try_push_field(L, LUA_TSTRING, table, field);
  if (!idx) return std::unexpected(idx.error());
  return pop_string(L);
}

Result<std::string> lua::try_get_string(lua_State* L, int table, unsigned field)
{
  auto idx = try_push_field(L, LUA_TSTRING, table, field);
  if (!idx) return std::unexpected(idx.error());
  return pop_string(L);
}

Result<lua_Integer> lua::try_get_integer(lua_State* L, int table, std::string_view field)
{
  auto idx = try_push_field(L, LUA_TNUMBER, table, field);
  if (!idx) return std::unexpected(idx.error());
  return pop_integer(L);
}

Result<lua_Integer> lua::try_get_integer(lua_State* L, int table, unsigned field)
{
  auto idx = try_push_field(L, LUA_TNUMBER, table, field);
  if (!idx) return std::unexpected(idx.error());
  return pop_integer(L);
}

Result<lua_Number> lua::try_get_number(lua_State* L, int table, std::string_view field)
{
  auto idx = try_push_field(L, LUA_TNUMBER, table, field);
  if (!idx) return std::unexpected(idx.error());
  return pop_number(L);
}

Result<lua_Number> lua::try_get_number(lua_State* L, int table, unsigned field)
{
  auto idx = try_push_field(L, LUA_TNUMBER, table, field);
  if (!idx) return std::unexpected(idx.error());
  return pop_number(L);
}

Result<bool> lua::try_get_boolean(lua_State* L, int table, std::string_view field)
{
  auto idx = try_push_field(L, LUA_TBOOLEAN, table, field);
  if (!idx) return std::unexpected(idx.error());
  return pop_boolean(L);
}

Result<bool> lua::try_get_boolean(lua_State* L, int table, unsigned field)
{
  auto idx = try_push_field(L, LUA_TBOOLEAN, table, field);
  if (!idx) return std::unexpected(idx.error());
  return pop_boolean(L);
}

Result<void> lua::do_file(lua_State* L, std::string_view path, unsigned argc, unsigned resultc)
{
  lua_pushcfunction(L, handle_error);
  int handler = lua_gettop(L);
  DEFER(lua_remove(L, handler));
  int error = luaL_loadfile(L, path.data());
  if (error != LUA_OK) [[unlikely]] { return std::unexpected<Error>(pop_string(L)); }
  error = lua_pcall(L, argc, resultc, handler);
  if (error != LUA_OK) [[unlikely]] { return std::unexpected<Error>(pop_string(L)); }
  return {};
}

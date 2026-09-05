#include "core/defer.hh"
#include "core/panic.hh"
#include "game/lua.hh"
#include <format>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

using namespace lua;

LuaReader::LuaReader(lua_State* L, i64 idx) noexcept : handle_{ L }, idx_{ idx }
{
  PRECONDITION(L != nullptr);
  PRECONDITION(idx != 0);
}

LuaReader::~LuaReader() noexcept
{
  DEBUG_ASSERT(lua_gettop(handle_) == idx_);
  lua_settop(handle_, idx_);
}

[[nodiscard]] Result<bool>        LuaReader::expect_boolean() const;
[[nodiscard]] Result<i64>         LuaReader::expect_integer() const;
[[nodiscard]] Result<f64>         LuaReader::expect_number() const;
[[nodiscard]] Result<std::string> LuaReader::expect_string() const;
[[nodiscard]] Result<LuaTable>    LuaReader::expect_table() const;

// {
//   lua_pushvalue(handle_, idx_);
//   i64 out = lua_tointeger(handle_, idx_);
//   lua_pop(handle_, 1);
//   return out;
// }

Result<bool> lua::try_get_boolean(lua_State* L, int table, std::string_view field)
{
  auto idx = try_push_field(L, LUA_TBOOLEAN, table, field);
  if (!idx) return std::unexpected(idx.error());
  return pop_boolean(L);
}

static int handle_error(lua_State* L)
{
  luaL_traceback(L, L, lua_tostring(L, -1), 1);
  return 1;
}

Error lua::make_content_error(lua_State* L, int found, int expected)
{
  return { std::format(
    "expected {}, found {}", lua_typename(L, expected), lua_typename(L, found)) };
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
  int error = luaL_loadfile(L, std::string(path).c_str());
  if (error != LUA_OK) [[unlikely]] { return std::unexpected<Error>(pop_string(L)); }
  error = lua_pcall(L, argc, resultc, handler);
  if (error != LUA_OK) [[unlikely]] { return std::unexpected<Error>(pop_string(L)); }
  return {};
}

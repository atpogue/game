#include "game/lua/table.hh"
#include "game/lua/util.hh"
#include <lua.h>
#include <sstream>
#include <variant>

std::string lua::path_string(LuaPath const& path, size_t len)
{
  std::stringstream out;
  for (size_t i = 0; i < len; ++i) {
    if (i > 0) out << '.';
    std::visit([&out](auto const& key) { out << key; }, path[i]);
  }
  return std::move(out).str();
}

std::string lua::path_string(LuaPath const& path) { return path_string(path, path.size()); }

void lua::push_key(lua_State* L, LuaKey const& key)
{
  std::visit(
    [L](auto const& value) {
      using Type = std::remove_cvref_t<decltype(value)>;
      if constexpr (std::same_as<Type, std::string_view>)
        lua_pushlstring(L, value.data(), value.size());
      else if constexpr (std::integral<Type>) lua_pushinteger(L, value);
      else static_assert(false, "unsupported lua key type");
    },
    key);
}

Result<bool> lua::try_pop_boolean(lua_State* L)
{
  auto type = lua_type(L, -1);
  if (type != LUA_TBOOLEAN) return lua::type_error(L, LUA_TBOOLEAN, type);
  auto out = lua_toboolean(L, -1);
  lua_pop(L, -1);
  return out;
}

Result<f64> lua::try_pop_number(lua_State* L)
{
  auto type = lua_type(L, -1);
  if (type != LUA_TNUMBER) return lua::type_error(L, LUA_TNUMBER, type);
  auto out = lua_tonumber(L, -1);
  lua_pop(L, -1);
  return out;
}

Result<std::string> lua::try_pop_string(lua_State* L) {}

Result<LuaTable> lua::try_pop_table(lua_State* L) {}

void push_key(lua_State* L, LuaKey const& key)
{
  std::visit(
    [L](auto const& value) {
      using Type = std::remove_cvref_t<decltype(value)>;
      if constexpr (std::same_as<Type, std::string_view>)
        lua_pushlstring(L, value.data(), value.size());
      else if constexpr (std::integral<Type>) lua_pushinteger(L, value);
      else static_assert(false, "unsupported lua key type");
    },
    key);
}


#include "core/panic.hh"
#include "sdk/query.hh"
#include <lua.hpp>

LuaQuery::LuaQuery(lua_State* handle, i32 idx) noexcept : handle_{ handle }, root_{ idx }, path_()
{
  PRECONDITION(handle != nullptr);
}

LuaQuery::LuaQuery(lua_State* handle, i32 idx, LuaKey key) noexcept
  : handle_{ handle }, root_{ idx }, path_()
{}

Result<LuaObject> LuaQuery::resolve() const
{
  push();
  auto ref = luaL_ref(handle(), LUA_REGISTRYINDEX);
  return LuaObject(handle(), ref);
}

void LuaQuery::append(LuaKey const& key) { path_.push_back(key); }

void LuaQuery::append(LuaPath const& path)
{
  for (auto const& key : path) append(key);
}

LuaQuery LuaQuery::operator[](LuaKey key) const { return LuaQuery(*this, key); }

LuaType LuaQuery::push() const
{
  auto ok = try_push();
  if (!ok) return LuaType::None;
  return *ok;
}

std::string describe_path(LuaPath const& path, size_t len)
{
  std::stringstream out;
  for (size_t i = 0; i < len; ++i) {
    if (i > 0) out << '.';
    std::visit([&out](auto const& key) { out << key; }, path[i]);
  }
  return std::move(out).str();
}

std::string describe_path(LuaPath const& path) { return path_string(path, path.size()); }

// Attempts to push onto the stack the value at the end of the path.
Result<LuaType> try_resolve_path() const
{
  lua_pushvalue(L, start);
  int idx = lua_gettop(L);

  for (size_t key = 0; key < path.size(); ++key) {
    if (auto type = lua_type(L, idx); type != LUA_TTABLE) {
      auto msg = std::format(
        "{}: expected {} but found {}", describe_path(path, idx + 1), lua_typename(L, LUA_TTABLE),
        lua_typename(L, type));
      lua_settop(L, idx - 1);
      return Error(std::move(msg));
    }
    lua::push_key(L, path[key]);
    lua_rawget(L, idx);
    lua_replace(L, idx);
  }

  if (lua_type(L, idx) == LUA_TNONE) {
    auto msg = std::format("{}: no value found", describe_path(path));
    lua_settop(L, idx - 1);
    return Error(std::move(msg));
  }

  return lua_type(L, -1);
}


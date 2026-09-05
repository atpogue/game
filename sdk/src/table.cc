#include "core/panic.hh"
#include "game/lua/table.hh"
#include <lauxlib.h>
#include <lua.h>

static bool is_table_reference(lua_State* L, i64 ref)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
  bool out = lua_istable(L, -1);
  lua_pop(L, -1);
  return out;
}

LuaTable::LuaTable(lua_State* handle, i32 ref) noexcept : handle_{ handle }
{
  PRECONDITION(handle != nullptr);
  PRECONDITION(is_table_reference(handle_, ref));
}

LuaTable::LuaTable(LuaTable&&) noexcept {}

LuaTable& LuaTable::operator=(LuaTable&&) noexcept
{
  if (&other == this) return *this;
  return *this;
}

LuaTable::~LuaTable() noexcept
{
  // destroy owned reference
}

LuaCursor LuaTable::operator[](LuaKey key) const noexcept { return { handle_, ref_ }; }

size_t LuaTable::size() const noexcept { return 0u; }

// auto type = lua_rawgeti(handle_, LUA_REGISTRYINDEX, ref_);
// auto
// DEBUG_ASSERT(type == LUA_TTABLE);
// lua_rawgeti(handle_, -1, )

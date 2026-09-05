#include "core/panic.hh"
#include "game/lua.hh"
#include "game/lua/object.hh"
#include "game/lua/util.hh"
#include <lauxlib.h>
#include <lua.h>

LuaObject::LuaObject(lua_State* L, i32 ref) noexcept : handle_{ L }, ref_{ ref }
{
  PRECONDITION(L != nullptr);
}

LuaObject::LuaObject(LuaObject&& other) noexcept : handle_{ other.handle_ }, ref_{ other.ref_ }
{
  other.handle_ = nullptr;
  other.ref_    = 0;
}

LuaObject::~LuaObject() noexcept { luaL_unref(handle_, LUA_REGISTRYINDEX, ref_); }

LuaObject& LuaObject::operator=(LuaObject&& other) noexcept
{
  if (&other == this) return *this;
  handle_       = other.handle_;
  ref_          = other.ref_;
  other.handle_ = nullptr;
  other.ref_    = 0;
  return *this;
}

Result<bool> LuaObject::expect_boolean() const
{
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, ref_);
  return lua::try_pop_boolean(handle_);
}

Result<f64> LuaObject::expect_number() const
{
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, ref_);
  return lua::try_pop_number(handle_);
}

Result<std::string> LuaObject::expect_string() const
{
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, ref_);
  return lua::try_pop_string(handle_);
}

Result<LuaTable> LuaObject::expect_table() const
{
  auto type = lua_rawgeti(handle_, LUA_REGISTRYINDEX, ref_);
  if (type != LUA_TTABLE) return lua::type_error(handle_, LUA_TTABLE, type);
  return LuaTable(handle_, luaL_ref(handle_, LUA_REGISTRYINDEX));
}

i32 LuaObject::type() const noexcept
{
  auto type = lua_rawgeti(handle_, LUA_REGISTRYINDEX, ref_);
  lua_pop(handle_, -1);
  return lua_;
}

bool LuaObject::is_nil() const noexcept { return type() == LUA_TNIL; }

bool LuaObject::is_none() const noexcept { return type() == LUA_TNONE; }

LuaType push() const { return luaL_unref(handle(), LUA_REGISTRYINDEX, reference_); }

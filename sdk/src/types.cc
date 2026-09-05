#include "core/panic.hh"
#include "sdk/types.hh"
#include <lua.hpp>

constexpr LuaType enumerate_lua_type(int t)
{
  switch (t) {
  case LUA_TNONE:
    return LuaType::None;
  case LUA_TNIL:
    return LuaType::Nil;
  case LUA_TBOOLEAN:
    return LuaType::Boolean;
  case LUA_TLIGHTUSERDATA:
    return LuaType::LightUserData;
  case LUA_TNUMBER:
    return LuaType::Number;
  case LUA_TSTRING:
    return LuaType::String;
  case LUA_TTABLE:
    return LuaType::Table;
  case LUA_TFUNCTION:
    return LuaType::Function;
  case LUA_TUSERDATA:
    return LuaType::UserData;
  case LUA_TTHREAD:
    return LuaType::Thread;
  }
  INVARIANT(false, "lua type not represented");
}


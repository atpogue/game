#pragma once
#include "core/result.hh"
#include "core/types.hh"
#include "sdk/path.hh"
#include "sdk/types.hh"
#include <lua.h>
#include <lua.hpp>
#include <string>

struct lua_State;

struct LuaValue;
struct LuaPath;
struct LuaNode;

// TODO: LuaTable and LuaValue::expect_table()
// TODO: LuaFunction and LuaValue::expect_function()
// TODO: LuaValue::expect_userdata()

namespace detail {
  constexpr LuaType type(int t);

  Error make_type_error(lua_State*, LuaType expected, LuaType found);

  [[nodiscard]] Result<LuaType> try_resolve_path(lua_State*, i32 start, LuaPath const& path);

  LuaType push_key(lua_State*, LuaKey const& key);

  [[nodiscard]] LuaValue    pop_value(lua_State*);
  [[nodiscard]] bool        pop_boolean(lua_State*);
  [[nodiscard]] f64         pop_number(lua_State*);
  [[nodiscard]] std::string pop_string(lua_State*);

  [[nodiscard]] Result<bool>        try_pop_boolean(lua_State*);
  [[nodiscard]] Result<f64>         try_pop_number(lua_State*);
  [[nodiscard]] Result<std::string> try_pop_string(lua_State*);

  [[nodiscard]] Result<LuaValue> find(LuaNode start, LuaPath);

  // void add_global_function(lua_State* L, char const* name, lua_CFunction fn);
  // void add_global_userdata(lua_State* L, char const* name, void* ptr);
  // void push_string(lua_State* L, std::string_view value);

  [[nodiscard]] Result<void>
  do_file(lua_State* L, std::string_view path, unsigned argc = 0u, unsigned resultc = 0u);
}

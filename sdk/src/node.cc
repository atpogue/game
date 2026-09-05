#include "core/panic.hh"
#include "sdk/detail.hh"
#include "sdk/node.hh"
#include "sdk/types.hh"
#include <format>
#include <lauxlib.h>
#include <lua.hpp>
#include <variant>

static std::string type_error(lua_State* L, int expected, int found)
{
  return std::format("expected {} but found {}", lua_typename(L, expected), lua_typename(L, found));
}

static void push_key(lua_State* L, LuaKey const& key)
{
  std::visit(
    [L](auto const& value) -> void {
      using Type = std::remove_cvref_t<decltype(value)>;
      if constexpr (std::same_as<Type, std::string>) lua_pushlstring(L, value.data(), value.size());
      else if constexpr (std::integral<Type>) lua_pushinteger(L, value);
      else static_assert(false, "unsupported lua key type");
    },
    key);
}

static Result<LuaType> try_resolve_path(lua_State* L, int start, LuaPath const& path)
{
  lua_pushvalue(L, start);

  for (size_t key = 0; key < path.size(); ++key) {
    if (lua_istable(L, -1)) {
      auto msg = std::format(
        "{}: {}", describe(path.span(key + 1)), type_error(L, LUA_TTABLE, lua_type(L, -1)));
      lua_pop(L, 1);
      return Error(std::move(msg));
    }
    push_key(L, path[key]);
    lua_rawget(L, -2);
    lua_replace(L, -2);
  }

  if (lua_isnone(L, -1)) {
    auto msg = std::format("{}: no value found", describe(path));
    lua_pop(L, 1);
    return Error(std::move(msg));
  }

  return LuaType{ lua_type(L, -1) };
}

static Result<LuaType> try_get_field(lua_State* L, int idx, LuaKey const& key)
{
  if (lua_istable(L, idx)) return Error(type_error(L, LUA_TTABLE, lua_type(L, idx)));
  push_key(L, key);
  auto type = lua_rawget(L, idx);
  return LuaType{ type };
}

static Result<bool> try_pop_boolean(lua_State* L)
{
  if (lua_isboolean(L, -1)) return Error(type_error(L, LUA_TBOOLEAN, lua_type(L, -1)));
  auto boolean = lua_toboolean(L, -1);
  lua_pop(L, 1);
  return boolean;
}

static Result<f64> try_pop_number(lua_State* L)
{
  if (lua_isnumber(L, -1)) return Error(type_error(L, LUA_TNUMBER, lua_type(L, -1)));
  auto number = lua_tonumber(L, -1);
  lua_pop(L, 1);
  return number;
}

static Result<std::string> try_pop_string(lua_State* L)
{
  if (lua_isstring(L, -1)) return Error(type_error(L, LUA_TSTRING, lua_type(L, -1)));
  auto string = lua_tostring(L, -1);
  lua_pop(L, 1);
  return string;
}

LuaNode::LuaNode(lua_State* L, i32 reference) noexcept
  : handle_{ L }, reference_{ reference }, type_{}
{
  PRECONDITION(L != nullptr);
  type_ = LuaType{ lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_) };
  lua_pop(handle_, 1);
}

LuaNode::LuaNode(LuaNode&& v) noexcept
  : handle_{ v.handle_ }, reference_{ v.reference_ }, type_{ v.type_ }
{
  v.reference_ = LUA_REFNIL;
  v.type_      = LuaType::None;
}

LuaNode& LuaNode::operator=(LuaNode&& v) noexcept
{
  if (&v == this) return *this;
  reference_   = v.reference_;
  handle_      = v.handle_;
  v.reference_ = LUA_REFNIL;
  v.type_      = LuaType::None;
  return *this;
}

LuaNode::~LuaNode() noexcept
{
  luaL_unref(handle_, LUA_REGISTRYINDEX, reference_);
  reference_ = LUA_REFNIL;
  type_      = LuaType::None;
}

Result<LuaNode> LuaNode::find(LuaKey const& key) const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  auto ok = try_get_field(handle_, -1, key);
  if (!ok) return err(ok);
  auto ref = luaL_ref(handle_, LUA_REGISTRYINDEX);
  return LuaNode(handle_, ref);
}

Result<LuaNode> LuaNode::find(LuaPath const& path) const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  auto ok = try_resolve_path(handle_, -1, path);
  if (!ok) return err(ok);
  auto ref = luaL_ref(handle_, LUA_REGISTRYINDEX);
  return LuaNode(handle_, ref);
}

Result<bool> LuaNode::expect_boolean() const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  return try_pop_boolean(handle_);
}

Result<f64> LuaNode::expect_number() const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  return try_pop_number(handle_);
}

Result<std::string> LuaNode::expect_string() const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  return try_pop_string(handle_);
}

Result<bool> LuaNode::expect_boolean(LuaKey const& key) const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  auto ok = try_get_field(handle_, -1, key);
  if (!ok) return err(ok);
  return try_pop_boolean(handle_);
}

Result<f64> LuaNode::expect_number(LuaKey const& key) const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  auto ok = try_get_field(handle_, -1, key);
  if (!ok) return err(ok);
  return try_pop_number(handle_);
}

Result<std::string> LuaNode::expect_string(LuaKey const& key) const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  auto ok = try_get_field(handle_, -1, key);
  if (!ok) return err(ok);
  return try_pop_string(handle_);
}

Result<bool> LuaNode::expect_boolean(LuaPath const& path) const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  auto ok = try_resolve_path(handle_, -1, path);
  if (!ok) return err(ok);
  return try_pop_boolean(handle_);
}

Result<f64> LuaNode::expect_number(LuaPath const& path) const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  auto ok = try_resolve_path(handle_, -1, path);
  if (!ok) return err(ok);
  return try_pop_number(handle_);
}

Result<std::string> LuaNode::expect_string(LuaPath const& path) const
{
  PRECONDITION(reference_ != LUA_REFNIL);
  lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_);
  auto ok = try_resolve_path(handle_, -1, path);
  if (!ok) return err(ok);
  return try_pop_string(handle_);
}


#include "sdk/node.hh"
#include "core/panic.hh"
#include <algorithm>
#include <format>
#include <lua.hpp>
#include <utility>
#include <variant>

namespace {

void push_key(lua_State* handle, LuaKey const& key)
{
  std::visit(
    [handle](auto const& value) {
      using Type = std::remove_cvref_t<decltype(value)>;
      if constexpr (std::same_as<Type, std::string>) {
        lua_pushlstring(handle, value.data(), value.size());
      } else {
        lua_pushinteger(handle, value);
      }
    },
    key);
}

std::string type_error(LuaPath const& path, LuaType expected, LuaType found)
{
  std::string const location = path.empty() ? "value" : describe(path);
  return std::format(
    "{}: expected {} but found {}", location, describe(expected), describe(found));
}

Result<i32> reference_child(
  lua_State* handle, i32 reference, LuaKey const& key, LuaPath const& parent_path,
  LuaPath const& child_path)
{
  LuaType const parent_type
    = static_cast<LuaType>(lua_rawgeti(handle, LUA_REGISTRYINDEX, reference));
  if (parent_type != LuaType::Table) {
    lua_pop(handle, 1);
    return Error(type_error(parent_path, LuaType::Table, parent_type));
  }

  push_key(handle, key);
  LuaType const child_type = static_cast<LuaType>(lua_rawget(handle, -2));
  lua_remove(handle, -2);

  if (child_type == LuaType::Nil) {
    lua_pop(handle, 1);
    return Error(std::format("{}: no value found", describe(child_path)));
  }

  return luaL_ref(handle, LUA_REGISTRYINDEX);
}

template <typename Type, typename Reader>
Result<Type> read_value(
  lua_State* handle, i32 reference, LuaPath const& path, LuaType expected, Reader read)
{
  LuaType const found = static_cast<LuaType>(lua_rawgeti(handle, LUA_REGISTRYINDEX, reference));
  if (found != expected) {
    lua_pop(handle, 1);
    return Error(type_error(path, expected, found));
  }

  Type value = read(handle);
  lua_pop(handle, 1);
  return value;
}

Result<LuaKey> read_key(lua_State* handle)
{
  switch (lua_type(handle, -2)) {
  case LUA_TNUMBER:
    if (!lua_isinteger(handle, -2)) {
      return Error("table contains a non-integer numeric key");
    }
    return LuaKey{ static_cast<i64>(lua_tointeger(handle, -2)) };
  case LUA_TSTRING: {
    size_t      length = 0;
    char const* data   = lua_tolstring(handle, -2, &length);
    return LuaKey{ std::string(data, length) };
  }
  default:
    return Error(std::format(
      "table contains unsupported {} key", lua_typename(handle, lua_type(handle, -2))));
  }
}

} // namespace

std::string describe(LuaNode const& value)
{
  return value.path().empty() ? "value" : describe(value.path());
}

LuaNode::LuaNode(std::shared_ptr<lua_State> owner, i32 reference, LuaPath path) noexcept
  : owner_{ std::move(owner) }, handle_{ owner_.get() }, reference_{ reference },
    type_{ LuaType::None }, path_(std::move(path))
{
  PRECONDITION(handle_ != nullptr);
  PRECONDITION(reference != LUA_NOREF && reference != LUA_REFNIL);
  type_ = static_cast<LuaType>(lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_));
  lua_pop(handle_, 1);
}

LuaNode::LuaNode(LuaNode&& other) noexcept
  : owner_{ std::move(other.owner_) }, handle_{ other.handle_ }, reference_{ other.reference_ },
    type_{ other.type_ },
    path_{ std::move(other.path_) }
{
  other.handle_    = nullptr;
  other.reference_ = LUA_NOREF;
  other.type_      = LuaType::None;
}

LuaNode& LuaNode::operator=(LuaNode&& other) noexcept
{
  if (&other == this) return *this;

  if (handle_ != nullptr && reference_ != LUA_NOREF && reference_ != LUA_REFNIL) {
    luaL_unref(handle_, LUA_REGISTRYINDEX, reference_);
  }

  owner_          = std::move(other.owner_);
  handle_         = other.handle_;
  reference_      = other.reference_;
  type_           = other.type_;
  path_           = std::move(other.path_);
  other.handle_    = nullptr;
  other.reference_ = LUA_NOREF;
  other.type_      = LuaType::None;
  return *this;
}

LuaNode::~LuaNode() noexcept
{
  if (handle_ != nullptr && reference_ != LUA_NOREF && reference_ != LUA_REFNIL) {
    luaL_unref(handle_, LUA_REGISTRYINDEX, reference_);
  }
}

Result<LuaNode> LuaNode::find(LuaKey const& key) const
{
  PRECONDITION(handle_ != nullptr);
  LuaPath const child_path = path_ + key;
  Result<i32>   reference  = reference_child(handle_, reference_, key, path_, child_path);
  if (!reference) return Error(std::move(reference).error());
  return LuaNode(owner_, *reference, child_path);
}

Result<LuaNode> LuaNode::find(LuaPath const& path) const
{
  PRECONDITION(handle_ != nullptr);
  if (path.empty()) return Error("cannot resolve an empty Lua path");

  Result<LuaNode> current = find(path[0]);
  for (u32 i = 1; current && i < path.size(); ++i) {
    current = current->find(path[i]);
  }
  return current;
}

Result<bool> LuaNode::expect_boolean() const
{
  PRECONDITION(handle_ != nullptr);
  return read_value<bool>(
    handle_, reference_, path_, LuaType::Boolean,
    [](lua_State* handle) { return lua_toboolean(handle, -1) != 0; });
}

Result<i64> LuaNode::expect_integer() const
{
  PRECONDITION(handle_ != nullptr);
  LuaType const found = static_cast<LuaType>(lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_));
  if (found != LuaType::Number || !lua_isinteger(handle_, -1)) {
    lua_pop(handle_, 1);
    std::string const location = path_.empty() ? "value" : describe(path_);
    return Error(std::format("{}: expected integer but found {}", location, describe(found)));
  }

  i64 const value = static_cast<i64>(lua_tointeger(handle_, -1));
  lua_pop(handle_, 1);
  return value;
}

Result<f64> LuaNode::expect_number() const
{
  PRECONDITION(handle_ != nullptr);
  return read_value<f64>(
    handle_, reference_, path_, LuaType::Number,
    [](lua_State* handle) { return static_cast<f64>(lua_tonumber(handle, -1)); });
}

Result<std::string> LuaNode::expect_string() const
{
  PRECONDITION(handle_ != nullptr);
  return read_value<std::string>(
    handle_, reference_, path_, LuaType::String,
    [](lua_State* handle) {
      size_t      length = 0;
      char const* data   = lua_tolstring(handle, -1, &length);
      return std::string(data, length);
    });
}

Result<bool> LuaNode::expect_boolean(LuaKey const& key) const
{
  Result<LuaNode> value = find(key);
  if (!value) return Error(std::move(value).error());
  return value->expect_boolean();
}

Result<i64> LuaNode::expect_integer(LuaKey const& key) const
{
  Result<LuaNode> value = find(key);
  if (!value) return Error(std::move(value).error());
  return value->expect_integer();
}

Result<f64> LuaNode::expect_number(LuaKey const& key) const
{
  Result<LuaNode> value = find(key);
  if (!value) return Error(std::move(value).error());
  return value->expect_number();
}

Result<std::string> LuaNode::expect_string(LuaKey const& key) const
{
  Result<LuaNode> value = find(key);
  if (!value) return Error(std::move(value).error());
  return value->expect_string();
}

Result<bool> LuaNode::expect_boolean(LuaPath const& path) const
{
  Result<LuaNode> value = find(path);
  if (!value) return Error(std::move(value).error());
  return value->expect_boolean();
}

Result<i64> LuaNode::expect_integer(LuaPath const& path) const
{
  Result<LuaNode> value = find(path);
  if (!value) return Error(std::move(value).error());
  return value->expect_integer();
}

Result<f64> LuaNode::expect_number(LuaPath const& path) const
{
  Result<LuaNode> value = find(path);
  if (!value) return Error(std::move(value).error());
  return value->expect_number();
}

Result<std::string> LuaNode::expect_string(LuaPath const& path) const
{
  Result<LuaNode> value = find(path);
  if (!value) return Error(std::move(value).error());
  return value->expect_string();
}

Result<std::vector<LuaKey>> LuaNode::keys() const
{
  PRECONDITION(handle_ != nullptr);
  LuaType const found = static_cast<LuaType>(lua_rawgeti(handle_, LUA_REGISTRYINDEX, reference_));
  if (found != LuaType::Table) {
    lua_pop(handle_, 1);
    return Error(type_error(path_, LuaType::Table, found));
  }

  std::vector<LuaKey> keys;
  lua_pushnil(handle_);
  while (lua_next(handle_, -2) != 0) {
    Result<LuaKey> key = read_key(handle_);
    lua_pop(handle_, 1);
    if (!key) {
      lua_pop(handle_, 2);
      return Error(std::format("{}: {}", describe(*this), std::move(key).error()));
    }
    keys.push_back(std::move(*key));
  }
  lua_pop(handle_, 1);

  std::ranges::sort(keys, [](LuaKey const& lhs, LuaKey const& rhs) {
    if (lhs.index() != rhs.index()) return lhs.index() < rhs.index();
    return std::visit(
      [](auto const& left, auto const& right) {
        using Left  = std::remove_cvref_t<decltype(left)>;
        using Right = std::remove_cvref_t<decltype(right)>;
        if constexpr (std::same_as<Left, Right>) return left < right;
        else return false;
      },
      lhs, rhs);
  });
  return keys;
}

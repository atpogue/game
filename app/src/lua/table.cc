#include "app/lua/table.hh"
#include "core/panic.hh"
#include <algorithm>
#include <format>
#include <lua.hpp>
#include <print>

namespace Lua {

  static void push_key(lua_State* L, Key key) noexcept
  {
    if (key.is_name()) {
      std::string_view const name = key.as_name();
      lua_pushlstring(L, name.data(), name.size());
    } else lua_pushinteger(L, key.as_index());
  }

  static bool is_identifier(std::string_view name) noexcept
  {
    auto const is_alpha
      = [](char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; };
    auto const is_alnum = [&](char c) { return is_alpha(c) || (c >= '0' && c <= '9'); };
    return !name.empty() && is_alpha(name.front()) && std::ranges::all_of(name, is_alnum);
  }

  // Formats the path of a field the way it would be written in Lua (i.e. `a.b["c-d"][1]`).
  static std::string field_path(std::string_view parent, Key key)
  {
    if (key.is_index()) return std::format("{}[{}]", parent, key.as_index());
    std::string_view const name = key.as_name();
    if (!is_identifier(name)) return std::format("{}[\"{}\"]", parent, name);
    if (parent.empty()) return std::string(name);
    return std::format("{}.{}", parent, name);
  }

  // Assigns the value pushed by the callable to the key, bypassing metamethods.
  template <typename PushValue>
  static void raw_set(Reference const& table, Key key, PushValue push_value) noexcept
  {
    lua_State* const L   = table.state();
    int const        idx = table.push();
    push_key(L, key);
    push_value(L);
    lua_rawset(L, idx);
    lua_pop(L, 1);
  }

  Table::Table(Reference reference, std::string path) noexcept
    : reference_(std::move(reference)), path_(std::move(path))
  {
    PRECONDITION(reference_.type() == Type::Table);
  }

  void Table::set(Key key, bool value) const noexcept
  {
    raw_set(reference_, key, [value](lua_State* L) { lua_pushboolean(L, value); });
  }

  void Table::set(Key key, Number value) const noexcept
  {
    raw_set(reference_, key, [value](lua_State* L) { lua_pushnumber(L, value); });
  }

  void Table::set(Key key, Integer value) const noexcept
  {
    raw_set(reference_, key, [value](lua_State* L) { lua_pushinteger(L, value); });
  }

  void Table::set(Key key, std::string_view value) const noexcept
  {
    raw_set(
      reference_, key, [value](lua_State* L) { lua_pushlstring(L, value.data(), value.size()); });
  }

  void Table::set(Key key, Reference const& value) const noexcept
  {
    PRECONDITION(value.state() == reference_.state(), "value belongs to a different state");
    raw_set(reference_, key, [&value](lua_State*) { value.push(); });
  }

  void Table::erase(Key key) const noexcept
  {
    raw_set(reference_, key, [](lua_State* L) { lua_pushnil(L); });
  }

  Value Table::operator[](Key key) const
  {
    lua_State* const L   = reference_.state();
    int const        idx = reference_.push();
    push_key(L, key);
    lua_rawget(L, idx);
    Reference field = Reference::pop(L);
    lua_pop(L, 1);
    return Value(std::move(field), field_path(path_, key));
  }

  std::vector<Key> Table::keys() const
  {
    std::vector<Integer>          indices;
    std::vector<std::string_view> names;

    lua_State* const L   = reference_.state();
    int const        idx = reference_.push();
    lua_pushnil(L);
    while (lua_next(L, idx) != 0) {
      // key at -2, value at -1
      if (lua_type(L, -2) == LUA_TSTRING) {
        size_t      length = 0;
        char const* data   = lua_tolstring(L, -2, &length); // does not convert; safe for lua_next
        names.emplace_back(data, length);
      } else if (lua_isinteger(L, -2)) {
        indices.push_back(lua_tointeger(L, -2));
      } else {
        std::println("{}: ignoring key of type {}", path_, luaL_typename(L, -2));
      }
      lua_pop(L, 1);
    }
    lua_pop(L, 1);

    std::ranges::sort(indices);
    std::ranges::sort(names);

    std::vector<Key> keys;
    keys.reserve(indices.size() + names.size());
    for (Integer index : indices)
      keys.emplace_back(index);
    for (std::string_view name : names)
      keys.emplace_back(name);
    return keys;
  }

} // namespace Lua

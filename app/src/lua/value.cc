#include "app/lua/value.hh"
#include "app/lua/table.hh"
#include "core/panic.hh"
#include <format>
#include <lua.hpp>
#include <print>

namespace Lua {

  Value::Value(Reference reference, std::string path) noexcept
    : reference_(std::move(reference)), path_(std::move(path))
  {}

  bool Value::to_boolean() const noexcept
  {
    DEBUG_ASSERT(is_boolean());
    lua_State* const L = reference_.state();
    reference_.push();
    bool const boolean = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return boolean;
  }

  Number Value::to_number() const noexcept
  {
    DEBUG_ASSERT(is_number() || is_integer());
    lua_State* const L = reference_.state();
    reference_.push();
    Number const number = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return number;
  }

  Integer Value::to_integer() const noexcept
  {
    DEBUG_ASSERT(is_integer());
    lua_State* const L = reference_.state();
    reference_.push();
    Integer const integer = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return integer;
  }

  std::string_view Value::to_string() const noexcept
  {
    DEBUG_ASSERT(is_string());
    lua_State* const L = reference_.state();
    reference_.push();
    size_t      length = 0;
    char const* data   = lua_tolstring(L, -1, &length);
    lua_pop(L, 1);
    // The registry reference keeps the string alive.
    return std::string_view(data, length);
  }

  Table Value::to_table() const
  {
    DEBUG_ASSERT(is_table());
    return Table(reference_, path_);
  }

  void Value::report(std::string_view expected) const
  {
    std::println("{}: expected {}, found {}", path_, expected, describe());
  }

  std::optional<bool> Value::expect_boolean() const
  {
    if (is_boolean()) return to_boolean();
    report(type_name(Type::Boolean));
    return std::nullopt;
  }

  std::optional<Integer> Value::expect_integer() const
  {
    if (is_integer()) return to_integer();
    if (is_number()) {
      // Accept floats with an exact integer representation (i.e. `2.0`).
      lua_State* const L = reference_.state();
      reference_.push();
      int           exact   = 0;
      Integer const integer = lua_tointegerx(L, -1, &exact);
      lua_pop(L, 1);
      if (exact) return integer;
    }
    report(type_name(Type::Integer));
    return std::nullopt;
  }

  std::optional<Integer> Value::expect_integer_range(Integer min, Integer max) const
  {
    std::optional<Integer> integer = expect_integer();
    if (!integer) return std::nullopt;
    if (*integer < min || *integer > max) {
      std::println("{}: expected integer in range [{}, {}], found {}", path_, min, max, *integer);
      return std::nullopt;
    }
    return integer;
  }

  std::optional<Number> Value::expect_number() const
  {
    if (is_number() || is_integer()) return to_number();
    report(type_name(Type::Number));
    return std::nullopt;
  }

  std::optional<Number> Value::expect_number_range(Number min, Number max) const
  {
    std::optional<Number> number = expect_number();
    if (!number) return std::nullopt;
    if (!(*number >= min && *number <= max)) {
      std::println("{}: expected number in range [{}, {}], found {}", path_, min, max, *number);
      return std::nullopt;
    }
    return number;
  }

  std::optional<std::string_view> Value::expect_string() const
  {
    if (is_string()) return to_string();
    report(type_name(Type::String));
    return std::nullopt;
  }

  std::optional<Table> Value::expect_table() const
  {
    if (is_table()) return to_table();
    report(type_name(Type::Table));
    return std::nullopt;
  }

  std::string Value::describe() const
  {
    std::string_view const name = type_name(type());
    switch (type()) {
    case Type::Boolean: return std::format("{} {}", name, to_boolean());
    case Type::Integer: return std::format("{} {}", name, to_integer());
    case Type::Number:  return std::format("{} {}", name, to_number());
    case Type::String:  return std::format("{} \"{}\"", name, to_string());
    default:            return std::string(name);
    }
  }

  bool read(bool& dst, Value const& src)
  {
    std::optional<bool> boolean = src.expect_boolean();
    if (!boolean) return false;
    dst = *boolean;
    return true;
  }

  bool read(Integer& dst, Value const& src)
  {
    std::optional<Integer> integer = src.expect_integer();
    if (!integer) return false;
    dst = *integer;
    return true;
  }

  bool read(Number& dst, Value const& src)
  {
    std::optional<Number> number = src.expect_number();
    if (!number) return false;
    dst = *number;
    return true;
  }

  bool read(std::string& dst, Value const& src)
  {
    std::optional<std::string_view> string = src.expect_string();
    if (!string) return false;
    dst = std::string(*string);
    return true;
  }

} // namespace Lua

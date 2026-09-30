#pragma once
#include "core/types.hh"
#include <string_view>

namespace Lua {

  // Mirrors the Lua C API type tags, with integers distinguished from other numbers.
  enum class Type : i8 {
    None          = -1,
    Nil           = 0,
    Boolean       = 1,
    LightUserData = 2,
    Number        = 3,
    String        = 4,
    Table         = 5,
    Function      = 6,
    UserData      = 7,
    Thread        = 8,
    Integer       = 9
  };

  [[nodiscard]] constexpr std::string_view type_name(Type type) noexcept
  {
    switch (type) {
    case Type::None:          return "none";
    case Type::Nil:           return "nil";
    case Type::Boolean:       return "boolean";
    case Type::LightUserData: return "light userdata";
    case Type::Number:        return "number";
    case Type::String:        return "string";
    case Type::Table:         return "table";
    case Type::Function:      return "function";
    case Type::UserData:      return "userdata";
    case Type::Thread:        return "thread";
    case Type::Integer:       return "integer";
    }
    return "none";
  }

  using Number  = f64;
  using Integer = i64;
} // namespace Lua

#pragma once
#include "core/types.hh"
#include <string>
#include <string_view>
#include <variant>

using LuaKey = std::variant<i64, std::string>;

enum class LuaType : int {
  None          = -1,
  Nil           = 0,
  Boolean       = 1,
  LightUserData = 2,
  Number        = 3,
  String        = 4,
  Table         = 5,
  Function      = 6,
  UserData      = 7,
  Thread        = 8
};

[[nodiscard]] constexpr std::string_view describe(LuaType type) noexcept
{
  switch (type) {
  case LuaType::None:
    return "none";
  case LuaType::Nil:
    return "nil";
  case LuaType::Boolean:
    return "boolean";
  case LuaType::LightUserData:
    return "light userdata";
  case LuaType::Number:
    return "number";
  case LuaType::String:
    return "string";
  case LuaType::Table:
    return "table";
  case LuaType::Function:
    return "function";
  case LuaType::UserData:
    return "userdata";
  case LuaType::Thread:
    return "thread";
  }
  return "unknown";
}

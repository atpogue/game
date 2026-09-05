#pragma once
#include "core/types.hh"
#include <concepts>
#include <span>
#include <string>
#include <variant>

using LuaKey = std::variant<int, std::string>;

using LuaQuery = std::variant<int, std::string_view, std::span<LuaKey const>>;

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

void describe(LuaType type);

// template <typename Type>
// concept LuaValue = requires {
//   std::integral<Type> || std::floating_point<Type> || std::same_as<Type, std::string>;
// };


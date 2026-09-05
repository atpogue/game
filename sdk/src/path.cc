#include "sdk/path.hh"
#include <concepts>
#include <sstream>
#include <string>
#include <type_traits>

std::string describe(LuaKey const& key)
{
  return std::visit(
    [](auto const& value) -> std::string {
      using Type = std::remove_cvref_t<decltype(value)>;
      if constexpr (std::same_as<Type, std::string>) return value;
      else if constexpr (std::integral<Type>) return std::to_string(value);
      else static_assert(false, "unsupported lua key type");
    },
    key);
}

std::string describe(LuaPath const& path) { return describe(path.span()); }

std::string describe(std::span<LuaKey const> path)
{
  std::stringstream out;
  for (u32 i = 0; i < path.size(); ++i) {
    std::visit(
      [&out, i](auto const& key) {
        using Type = std::remove_cvref_t<decltype(key)>;
        if constexpr (std::same_as<Type, std::string>) {
          if (i > 0) out << '.';
          out << key;
        } else out << '[' << key << ']';
      },
      path[i]);
  }
  return std::move(out).str();
}

#include "sdk/path.hh"
#include <sstream>
#include <string>

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
  for (size_t i = 0; i < path.size(); ++i) {
    if (i > 0) out << '.';
    std::visit([&out](auto const& key) { out << key; }, path[i]);
  }
  return std::move(out).str();
}


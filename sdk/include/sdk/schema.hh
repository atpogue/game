#pragma once
#include "core/types.hh"
#include "sdk/node.hh"
#include "sdk/path.hh"
#include <concepts>
#include <expected>
#include <functional>
#include <string>
#include <type_traits>

template <typename Type>
using LuaParser = std::function<Status(LuaNode const&, Type&)>;

template <typename Type>
struct LuaField
{
  LuaKey key;
  std::variant<
    f32         Type::*,
    f64         Type::*,
    u8          Type::*,
    u16         Type::*,
    u32         Type::*,
    u64         Type::*,
    i8          Type::*,
    i16         Type::*,
    i32         Type::*,
    i64         Type::*,
    bool        Type::*,
    std::string Type::*,
    LuaParser<Type>>
    member;
};

template <typename Type>
struct LuaTraits
{};

struct Rectangle
{
  f32 x, y, w, h;
};

template <typename Type>
using LuaSchema = std::span<LuaField<Type> const>;

template <>
struct LuaTraits<Rectangle>
{
  static constexpr LuaField<Rectangle> schema[] = {
    { "x", &Rectangle::x },
    { "y", &Rectangle::y },
    { "w", &Rectangle::w },
    { "h", &Rectangle::h },
  };
};

template <typename Type>
concept LuaTable = requires {
  { LuaTraits<Type>::schema } -> std::convertible_to<LuaSchema<Type>>;
};

template <typename Type>
concept LuaValue = requires {
  std::integral<Type> || std::floating_point<Type> || std::same_as<Type, std::string>;
};

template <typename Class, typename Member>
LuaField<Class> make_nested_field(LuaKey key, Member Class::* member)
{
  return {
    std::move(key),
    [member](LuaNode const& node, Class& out) -> Status {
      auto ok = parse<Member>(node, out.*member);
      if (!ok) return ok;
      return {};
    },
  };
}

template <LuaTable Type>
Status expect_table(LuaNode const& node, Type& out)
{
  for (auto const& field : (LuaSchema<Type>)LuaTraits<Type>::schema) {
    auto ok = std::visit(
      [&](auto member) -> Status {
        using Member = std::remove_cvref_t<decltype(out.*member)>;

        return parse(node, field.key, out.*member);
      },
      field.member);
    if (!ok) return ok;
  }
  return {};
}


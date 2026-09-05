#pragma once
#include "core/result.hh"
#include "core/types.hh"
#include "sdk/node.hh"
#include "sdk/types.hh"
#include <concepts>
#include <functional>
#include <span>

template <typename Type>
struct LuaTraits
{};

// Not actually a "parser"? What about "compiler"? "Translator"? "Mapper"?
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
using LuaSchema = std::span<LuaField<Type> const>;

template <typename Type>
concept LuaTable = requires {
  { LuaTraits<Type>::schema } -> std::convertible_to<LuaSchema<Type>>;
};

template <typename Type>
concept LuaValue = requires {
  std::integral<Type> || std::floating_point<Type> || std::same_as<Type, std::string>;
};

template <typename Type>
Status translate(LuaNode const& node, Type& out);

template <LuaTable Type>
Status translate_table(LuaNode const& node, LuaKey key, Type& out)
{
  for (auto const& field : (LuaSchema<Type>)LuaTraits<Type>::schema) {
    auto ok = std::visit(
      [&](auto member) -> Status {
        // using Member = std::remove_cvref_t<decltype(out.*member)>;
        return translate(out.*member, field.key);
      },
      field.member);
    if (!ok) return ok;
  }
  return {};
}

template <LuaValue Type>
Status translate_value(LuaNode const& node, LuaKey key, Type& out)
{
  if (auto value = node.expect<Type>(key)) out = std::move(*value);
  else return Error(std::move(value).error());
  return {};
}

template <typename Type>
Status translate(LuaNode const& node, Type& out)
{
  if constexpr (LuaValue<Type>) return translate_value(node, out);
  else if constexpr (LuaTable<Type>) return translate_table(node, out);
  else static_assert(false, "unsupported type");
}

template <typename Class, typename Member>
Status nested_table(Member Class::* member)
{
  return [member](LuaNode const& node, Class& out) -> Status {
    auto ok = translate<Member>(node, out.*member);
    if (!ok) return ok;
    return {};
  };
}

struct Rectangle
{
  f32 x, y, w, h;
};

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

struct Sprite
{
  std::string atlas;
  Rectangle   source;
  u32         tint;
};

template <>
struct LuaTraits<Sprite>
{
  static constexpr LuaField<Rectangle> schema[] = {
    { "atlas", &Sprite::atlas },
    { "source", nested_table(&Sprite::source) },
    { "tint", &Sprite::tint },
  };
};


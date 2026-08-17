#pragma once
#include "core/error.hh"
#include "core/types.hh"
#include <concepts>
#include <expected>
#include <format>
#include <lua.hpp>
#include <span>
#include <string>
#include <variant>

// helper functions for Lua's C API
namespace lua {
  [[nodiscard]] Error make_content_error(lua_State* L, int found, int expected);
  void                add_global_function(lua_State* L, char const* name, lua_CFunction fn);
  void                add_global_userdata(lua_State* L, char const* name, void* ptr);
  void                push_string(lua_State* L, std::string_view value);

  template <typename... Args>
  void push_fstring(lua_State* L, std::format_string<Args...> fmt, Args&&... args)
  {
    auto str = std::format(fmt, std::forward<Args>(args)...);
    lua_pushstring(L, str.data());
  }

  [[nodiscard]] std::string pop_string(lua_State* L);
  [[nodiscard]] lua_Integer pop_integer(lua_State* L);
  [[nodiscard]] lua_Number  pop_number(lua_State* L);
  [[nodiscard]] bool        pop_boolean(lua_State* L);

  // Only pushes onto the stack if the expected type is found.
  // Returns: absolute index of the new item, or a content error if the expected
  // type wasn't found. Assumes: [L] is not null, [idx] is non-zero, value at
  // [idx] is a table, [field] is non-zero.
  [[nodiscard]] Result<int> try_push_field(lua_State* L, int type, int idx, unsigned field);

  // Only pushes onto the stack if the expected type is found.
  // Returns: absolute index of the new item, or a content error if the expected
  // type wasn't found. Assumes: [L] is not null, [idx] is non-zero, value at
  // [idx] is a table
  [[nodiscard]] Result<int> try_push_field(lua_State* L, int type, int idx, std::string_view field);
  [[nodiscard]] Result<bool>        try_get_boolean(lua_State* L, int idx, unsigned field);
  [[nodiscard]] Result<bool>        try_get_boolean(lua_State* L, int idx, std::string_view field);
  [[nodiscard]] Result<std::string> try_get_string(lua_State* L, int idx, unsigned field);
  [[nodiscard]] Result<std::string> try_get_string(lua_State* L, int idx, std::string_view field);
  [[nodiscard]] Result<lua_Integer> try_get_integer(lua_State* L, int idx, unsigned field);
  [[nodiscard]] Result<lua_Integer> try_get_integer(lua_State* L, int idx, std::string_view field);
  [[nodiscard]] Result<lua_Number>  try_get_number(lua_State* L, int idx, unsigned field);
  [[nodiscard]] Result<lua_Number>  try_get_number(lua_State* L, int idx, std::string_view field);

  template <typename Type>
  Result<Type> try_get(lua_State* L, int idx, std::string_view field)
  {
    if constexpr (std::same_as<Type, bool>) return try_get_boolean(L, idx, field);
    else if constexpr (std::integral<Type>)
      return try_get_integer(L, idx, field).transform([](auto value) {
        return static_cast<Type>(value);
      });
    else if constexpr (std::floating_point<Type>)
      return try_get_number(L, idx, field).transform([](auto value) {
        return static_cast<Type>(value);
      });
    else if constexpr (std::same_as<Type, std::string>) return try_get_string(L, idx, field);
    else static_assert(false, "unsupported field type");
  }

  template <typename Type>
  struct Field
  {
    std::string_view name;
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
      std::string Type::*>
      member;
  };

  // TODO: subtable fields?
  /* This is supposed to reduce the amount of lua parsing code needed for
  definitions constexpr Field<Rectangle> fields[] = { { "x", &Rectangle::x }, {
  "y", &Rectangle::y }, { "w", &Rectangle::w }, { "h", &Rectangle::h },
  };

  auto result = try_get_fields(L, idx, fields);
  */
  template <typename Type>
  using Schema = std::span<Field<Type> const>;

  template <typename Type>
  Result<Type> try_get_fields(lua_State* L, int idx, Schema<Type> schema)
  {
    Type out;
    for (auto const& field : schema) {
      auto result = std::visit(
        [&](auto member) -> Result<void> {
          using Member = std::remove_cvref_t<decltype(out.*member)>;
          auto value   = try_get<Member>(L, idx, field.name);
          if (!value) return std::unexpected(value.error());
          out.*member = std::move(*value);
          return {};
        },
        field.member
      );
      if (!result) return std::unexpected(result.error());
    }
    return out;
  }

  [[nodiscard]] Result<void>
  do_file(lua_State* L, std::string_view path, unsigned argc = 0u, unsigned resultc = 0u);
} // namespace lua

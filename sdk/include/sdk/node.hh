#pragma once
#include "core/result.hh"
#include "core/types.hh"
#include "sdk/path.hh"
#include "sdk/types.hh"
#include <concepts>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct lua_State;
struct LuaNode;

std::string describe(LuaNode const& value);

struct LuaNode
{
  LuaNode()                          = delete;
  LuaNode(LuaNode const&)            = delete;
  LuaNode& operator=(LuaNode const&) = delete;

  LuaNode(LuaNode&&) noexcept;
  LuaNode& operator=(LuaNode&&) noexcept;
  ~LuaNode() noexcept;

  [[nodiscard]] Result<LuaNode> find(LuaKey const&) const;
  [[nodiscard]] Result<LuaNode> find(LuaPath const& path) const;

  [[nodiscard]] Result<bool>        expect_boolean() const;
  [[nodiscard]] Result<i64>         expect_integer() const;
  [[nodiscard]] Result<f64>         expect_number() const;
  [[nodiscard]] Result<std::string> expect_string() const;

  [[nodiscard]] Result<bool>        expect_boolean(LuaKey const&) const;
  [[nodiscard]] Result<i64>         expect_integer(LuaKey const&) const;
  [[nodiscard]] Result<f64>         expect_number(LuaKey const&) const;
  [[nodiscard]] Result<std::string> expect_string(LuaKey const&) const;

  [[nodiscard]] Result<bool>        expect_boolean(LuaPath const&) const;
  [[nodiscard]] Result<i64>         expect_integer(LuaPath const&) const;
  [[nodiscard]] Result<f64>         expect_number(LuaPath const&) const;
  [[nodiscard]] Result<std::string> expect_string(LuaPath const&) const;

  template <typename Type>
  [[nodiscard]] Result<Type> expect() const
  {
    if constexpr (std::same_as<Type, bool>) return expect_boolean();
    else if constexpr (std::integral<Type>) {
      Result<i64> value = expect_integer();
      if (!value) return Error(std::move(value).error());
      if (!std::in_range<Type>(*value)) {
        return Error(describe(*this) + ": integer is outside the destination type's range");
      }
      return static_cast<Type>(*value);
    } else if constexpr (std::floating_point<Type>)
      return expect_number().transform([](auto value) {
        return static_cast<Type>(value);
      });
    else if constexpr (std::same_as<Type, std::string>)
      return expect_string();
    else static_assert(always_false<Type>, "unsupported type");
  }

  template <typename Type>
  [[nodiscard]] Result<Type> expect(LuaKey const& key) const
  {
    Result<LuaNode> value = find(key);
    if (!value) return Error(std::move(value).error());
    return value->expect<Type>();
  }

  template <typename Type>
  [[nodiscard]] Result<Type> expect(LuaPath const& path) const
  {
    Result<LuaNode> value = find(path);
    if (!value) return Error(std::move(value).error());
    return value->expect<Type>();
  }

  [[nodiscard]] Result<std::vector<LuaKey>> keys() const;

  [[nodiscard]] LuaType type() const noexcept { return type_; }

  [[nodiscard]] LuaPath const& path() const noexcept { return path_; }

private:
  friend struct LuaState;

  template <typename>
  static constexpr bool always_false = false;

  LuaNode(std::shared_ptr<lua_State> owner, i32 reference, LuaPath path) noexcept;

  std::shared_ptr<lua_State> owner_;
  lua_State* handle_;
  i32        reference_;
  LuaType    type_;
  LuaPath    path_;
};

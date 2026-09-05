#pragma once
#include "core/result.hh"
#include "core/types.hh"
#include "sdk/path.hh"
#include "sdk/types.hh"

struct lua_State;
struct LuaNode;

std::string describe(LuaNode const& value);

struct LuaNode
{
  LuaNode()                          = delete;
  LuaNode(LuaNode const&)            = delete;
  LuaNode& operator=(LuaNode const&) = delete;

  LuaNode(lua_State* L, i32 reference) noexcept;
  LuaNode(LuaNode&&) noexcept;
  LuaNode& operator=(LuaNode&&) noexcept;
  ~LuaNode() noexcept;

  [[nodiscard]] Result<LuaNode> find(LuaKey const&) const;
  [[nodiscard]] Result<LuaNode> find(LuaPath const& path) const;

  [[nodiscard]] bool        as_boolean() const;
  [[nodiscard]] f64         as_number() const;
  [[nodiscard]] std::string as_string() const;

  [[nodiscard]] Result<bool>        expect_boolean() const;
  [[nodiscard]] Result<f64>         expect_number() const;
  [[nodiscard]] Result<std::string> expect_string() const;

  [[nodiscard]] Result<bool>        expect_boolean(LuaKey const&) const;
  [[nodiscard]] Result<f64>         expect_number(LuaKey const&) const;
  [[nodiscard]] Result<std::string> expect_string(LuaKey const&) const;

  [[nodiscard]] Result<bool>        expect_boolean(LuaPath const&) const;
  [[nodiscard]] Result<f64>         expect_number(LuaPath const&) const;
  [[nodiscard]] Result<std::string> expect_string(LuaPath const&) const;

  template <LuaValue Type, typename... Args>
  [[nodiscard]] Result<Type> expect(Args&&... args) const
  {
    if constexpr (std::same_as<Type, bool>) return expect_boolean(std::forward<Args>(args)...);
    else if constexpr (std::integral<Type> || std::floating_point<Type>)
      return expect_number(std::forward<Args>(args)...).transform([](auto value) {
        return static_cast<Type>(value);
      });
    else if constexpr (std::same_as<Type, std::string>)
      return expect_string(std::forward<Args>(args)...);
    else static_assert(false, "unsupported type");
  }

  [[nodiscard]] LuaType type() const noexcept { return type_; }

  [[nodiscard]] lua_State* handle() const noexcept { return handle_; }

private:
  lua_State* handle_;
  i32        reference_;
  LuaType    type_;
};


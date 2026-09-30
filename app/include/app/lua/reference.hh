#pragma once
#include "app/lua/types.hh"

struct lua_State;

namespace Lua {

  // Owns a slot in the Lua registry that keeps a value alive.
  // Must not outlive the state that created it.
  struct Reference
  {
    // Pops the value on the top of the stack into a new reference.
    [[nodiscard]] static Reference pop(lua_State* L) noexcept;

    Reference() noexcept = default;
    Reference(Reference const&) noexcept;
    Reference(Reference&&) noexcept;
    Reference& operator=(Reference const&) noexcept;
    Reference& operator=(Reference&&) noexcept;
    ~Reference() noexcept;

    // Returns true if both refer to the same Lua value (raw equality).
    friend bool operator==(Reference const& l, Reference const& r) noexcept;

    [[nodiscard]] Type type() const noexcept { return type_; }

    [[nodiscard]] lua_State* state() const noexcept { return state_; }

    // Pushes the referenced value onto the top of the stack and returns its stack index.
    int push() const noexcept;

  private:

    Reference(lua_State* L, int ridx, Type type) noexcept;

    void release() noexcept;

    static constexpr int no_ref = -2; // LUA_NOREF

    lua_State* state_ = nullptr; // non-owning pointer
    int        ridx_  = no_ref;
    Type       type_  = Type::None;
  };

} // namespace Lua

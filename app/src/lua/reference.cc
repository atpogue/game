#include "app/lua/reference.hh"
#include "core/panic.hh"
#include <lua.hpp>

namespace Lua {

  static Type type_at(lua_State* L, int idx) noexcept
  {
    if (lua_isinteger(L, idx)) return Type::Integer;
    return static_cast<Type>(lua_type(L, idx));
  }

  Reference Reference::pop(lua_State* L) noexcept
  {
    static_assert(no_ref == LUA_NOREF);
    PRECONDITION(L != nullptr);
    Type const type = type_at(L, -1);
    int const  ridx = luaL_ref(L, LUA_REGISTRYINDEX);
    return Reference(L, ridx, type);
  }

  Reference::Reference(lua_State* L, int ridx, Type type) noexcept
    : state_{ L }, ridx_{ ridx }, type_{ type }
  {}

  Reference::Reference(Reference const& other) noexcept : type_{ other.type_ }
  {
    if (other.state_ == nullptr) return;
    state_ = other.state_;
    lua_rawgeti(state_, LUA_REGISTRYINDEX, other.ridx_);
    ridx_ = luaL_ref(state_, LUA_REGISTRYINDEX);
  }

  Reference::Reference(Reference&& other) noexcept
    : state_{ other.state_ }, ridx_{ other.ridx_ }, type_{ other.type_ }
  {
    other.state_ = nullptr;
    other.ridx_  = no_ref;
    other.type_  = Type::None;
  }

  Reference& Reference::operator=(Reference const& other) noexcept
  {
    if (&other == this) return *this;
    Reference copy(other);
    return *this = std::move(copy);
  }

  Reference& Reference::operator=(Reference&& other) noexcept
  {
    if (&other == this) return *this;
    release();
    state_       = other.state_;
    ridx_        = other.ridx_;
    type_        = other.type_;
    other.state_ = nullptr;
    other.ridx_  = no_ref;
    other.type_  = Type::None;
    return *this;
  }

  Reference::~Reference() noexcept { release(); }

  void Reference::release() noexcept
  {
    if (state_ != nullptr && ridx_ != LUA_NOREF) luaL_unref(state_, LUA_REGISTRYINDEX, ridx_);
    state_ = nullptr;
    ridx_  = no_ref;
    type_  = Type::None;
  }

  bool operator==(Reference const& l, Reference const& r) noexcept
  {
    if (l.state_ != r.state_) return false;
    if (l.state_ == nullptr || l.ridx_ == r.ridx_) return true;
    lua_State* const L = l.state_;
    lua_rawgeti(L, LUA_REGISTRYINDEX, l.ridx_);
    lua_rawgeti(L, LUA_REGISTRYINDEX, r.ridx_);
    bool const equal = lua_rawequal(L, -1, -2);
    lua_pop(L, 2);
    return equal;
  }

  int Reference::push() const noexcept
  {
    PRECONDITION(state_ != nullptr, "pushed an empty reference");
    lua_rawgeti(state_, LUA_REGISTRYINDEX, ridx_);
    DEBUG_ASSERT(type_ == type_at(state_, -1));
    return lua_gettop(state_);
  }

} // namespace Lua

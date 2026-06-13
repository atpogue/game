#pragma once
#include "core/types.hh"
#include <bitset>
#include <concepts>
#include <type_traits>

struct Nothing
{
};

template <typename T>
concept TypeInfo = requires {
  { T::count() } -> std::convertible_to<u32>;
  { T::template index<Nothing>() } -> std::convertible_to<u32>;
  { T::template has<Nothing>() } -> std::convertible_to<bool>;
};

template <typename... Types> struct TypeList
{ ///////////////////////////////////////////////////////////////////

  TypeList() = delete;

  static constexpr u32 count() { return sizeof...(Types); }

  template <typename Type> static constexpr bool has()
  {
    return (std::is_same_v<Type, Types> || ...);
  }

  template <typename Type> static constexpr u32 index()
  {
    static_assert(has<Type>(), "type not in TypeList");
    u32 i = 0;
    // short-circuits on first match; i lands on the matching position
    (void)((std::is_same_v<Type, Types> || (++i, false)) || ...);
    return i;
  }

  template <typename Function> static constexpr void for_each(Function&& fn)
  {
    (fn.template operator()<Types>(), ...);
  }

}; //////////////////////////////////////////////////////////////////////////////////

template <typename... Ts> struct TypeFlag : private std::bitset<sizeof...(Ts)>
{ //////////////////////////////
private:

  using Base = std::bitset<sizeof...(Ts)>;

public:

  using Types = TypeList<Ts...>;

  using Base::any;
  using Base::count;
  using Base::none;
  using Base::reset;
  using Base::set;
  using Base::size;
  using Base::operator==;

  template <typename U, typename... Us> constexpr void set(bool value = true)
  {
    Base::set(Types::template index<U>(), value);
    (Base::set(Types::template index<Us>(), value), ...);
  }

  constexpr bool has(u32 i) const { return Base::test(i); }

  constexpr bool has(const TypeFlag& other) const
  {
    return ((Base)(*this) & (Base)(other)) == (Base)(other);
  }

  template <typename U, typename... Us> constexpr bool has()
  {
    return Base::test(Types::template index<U>())
        && (Base::test(Types::template index<Us>()) && ...);
  }

}; //////////////////////////////////////////////////////////////////////////////////


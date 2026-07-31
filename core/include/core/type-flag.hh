#pragma once
#include "core/type-list.hh"
#include <bitset>

template <typename... Ts>
struct TypeFlag : private std::bitset<sizeof...(Ts)>
{
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

  template <typename U, typename... Us>
  constexpr void set(bool value = true)
  {
    Base::set(Types::template index<U>(), value);
    (Base::set(Types::template index<Us>(), value), ...);
  }

  constexpr bool has(u32 i) const { return Base::test(i); }

  constexpr bool has(TypeFlag const& other) const
  {
    return ((Base)(*this) & (Base)(other)) == (Base)(other);
  }

  template <typename U, typename... Us>
  constexpr bool has()
  {
    return Base::test(Types::template index<U>())
        && (Base::test(Types::template index<Us>()) && ...);
  }
};

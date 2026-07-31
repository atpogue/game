#pragma once
#include "core/types.hh"
#include <type_traits>

template <typename... Types>
struct TypeList
{
  TypeList() = delete;

  static constexpr u32 count() { return sizeof...(Types); }

  template <typename Type>
  static constexpr bool has()
  {
    return (std::is_same_v<Type, Types> || ...);
  }

  template <typename Type>
  static constexpr u32 index()
  {
    static_assert(has<Type>(), "type not in TypeList");
    u32 i = 0;
    // short-circuits on first match; i lands on the matching position
    (void)((std::is_same_v<Type, Types> || (++i, false)) || ...);
    return i;
  }

  template <typename Function>
  static constexpr void for_each(Function&& fn)
  {
    (fn.template operator()<Types>(), ...);
  }

  template <template <typename...> typename Container>
  using fill = Container<Types...>;

  // TODO: append
};


#pragma once
#include "engine/core/types.hh"
#include <type_traits>

template <typename... Types>
struct TypeList {

    TypeList() = delete;

    static constexpr u32 size = sizeof...(Types);

    template <typename Type>
    static constexpr bool contains = (std::is_same_v<Type, Types> || ...);

template <typename Type>
static constexpr u32 index() {
    static_assert(contains<Type>, "type not in TypeList");
    u32 i = 0;
    // short-circuits on first match; i lands on the matching position
    ((std::is_same_v<Type, Types> ? true : (++i, false)) || ...);
    return i;
}

    template <typename Function>
    static constexpr void for_each(Function&& fn) {
        (fn.template operator()<Types>(), ...);
    }

};


#pragma once
#include <type_traits>

#define DEFINE_ENUM_BITWISE_OPERATORS(T)                                                \
    constexpr T operator|(T a, T b) noexcept {                                          \
        using U = std::underlying_type_t<T>;                                            \
        return static_cast<T>(static_cast<U>(a) | static_cast<U>(b));                   \
    }                                                                                   \
    constexpr T operator&(T a, T b) noexcept {                                          \
        using U = std::underlying_type_t<T>;                                            \
        return static_cast<T>(static_cast<U>(a) & static_cast<U>(b));                   \
    }                                                                                   \
    constexpr T operator^(T a, T b) noexcept {                                          \
        using U = std::underlying_type_t<T>;                                            \
        return static_cast<T>(static_cast<U>(a) ^ static_cast<U>(b));                   \
    }                                                                                   \
    constexpr T operator~(T a) noexcept {                                               \
        using U = std::underlying_type_t<T>;                                            \
        return static_cast<T>(~static_cast<U>(a));                                      \
    }                                                                                   \
    constexpr T& operator|=(T& a, T b) noexcept { return a = a | b; }                   \
    constexpr T& operator&=(T& a, T b) noexcept { return a = a & b; }                   \
    constexpr T& operator^=(T& a, T b) noexcept { return a = a ^ b; }                   \
    constexpr bool has_flag(T mask, T flag) noexcept {                                  \
        using U = std::underlying_type_t<T>;                                            \
        return (static_cast<U>(mask) & static_cast<U>(flag)) == static_cast<U>(flag);   \
    }


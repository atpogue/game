#pragma once
#include <concepts>

template <std::invocable F>
struct Defer
{
  F fn;

  ~Defer() { fn(); }
};

// deduction guide
template <typename F>
Defer(F) -> Defer<F>;

#define DEFER_CONCAT_IMPL(a, b) a##b
#define DEFER_CONCAT(a, b)      DEFER_CONCAT_IMPL(a, b)

#define DEFER(expr)                       \
  Defer DEFER_CONCAT(defer_, __COUNTER__) \
  {                                       \
    [&] { expr; }                         \
  }

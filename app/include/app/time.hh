#pragma once

// TODO: adopt std::chrono over SDL time

#include "core/types.hh"

using Hertz       = u64;
using Nanoseconds = u64;

// hertz to nanoseconds
[[nodiscard]] constexpr Nanoseconds from_hertz(Hertz time) noexcept { return 1'000'000'000 / time; }

// nanoseconds to seconds
[[nodiscard]] constexpr f32 to_seconds(Nanoseconds time) noexcept
{
  return f32(time) / 1'000'000'000.f;
}

struct TimeStep
{
  constexpr explicit TimeStep(u64 size) noexcept : size_{ size }, lag_{ 0u }, age_{ 0u }
  {
    // allow a step size of 0?
  }

  // Returns how many time steps to run this frame.
  [[nodiscard]] u64 advance(u64 dt) noexcept
  {
    if (size_ == 0u) return 0u;
    lag_      += dt;
    u64 steps  = lag_ / size_;
    lag_      -= steps * size_;
    age_      += steps;
    return steps;
  }

  [[nodiscard]] u64 lag() const noexcept { return lag_; }

  [[nodiscard]] u64 age() const noexcept { return age_; }

  [[nodiscard]] f32 alpha() const noexcept { return f32(lag_) / f32(size_); }

  [[nodiscard]] constexpr u64 size() const noexcept { return size_; }

  void resize(u64 size)
  {
    size_ = size;
    lag_  = 0.f;
    age_  = 0u;
  }

  void reset()
  {
    lag_ = 0.f;
    age_ = 0u;
  }

private:

  // made with nanoseconds in mind by the unit is arbitrary
  u64 size_; // this should not change after construction unless reseting
  u64 lag_;
  u64 age_;
};

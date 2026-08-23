#pragma once
#include "core/panic.hh"
#include "core/types.hh"
#include <chrono>

struct Clock
{
  using Duration  = std::chrono::nanoseconds;
  using TimePoint = std::chrono::steady_clock::time_point;

  Clock(Clock const&) noexcept            = default;
  Clock(Clock&&)                          = delete;
  Clock& operator=(Clock const&) noexcept = default;
  Clock& operator=(Clock&&)               = delete;
  ~Clock() noexcept                       = default;

  constexpr Clock(i64 tick, u32 rate) noexcept
    : tick_{ tick }
    , period_(from_hertz(rate))
    , delta_(0)
    , lag_(0)
    , prior_(std::chrono::steady_clock::now())
  {
    PRECONDITION(tick >= 0);
    PRECONDITION(rate > 0);
  }

  // Updates the clock with the current time and returns how many ticks have passed.
  [[nodiscard]] i64 advance() noexcept
  {
    DEBUG_ASSERT(period_ > Duration::zero());
    auto const now    = std::chrono::steady_clock::now();
    delta_            = now - prior_;
    lag_             += delta_;
    auto const ticks  = lag_ / period_;
    lag_             %= period_;
    tick_            += ticks;
    prior_            = now;
    return ticks;
  }

  // Gets the age of the clock in ticks at the time of the last advance.
  [[nodiscard]] i64 tick() const noexcept { return tick_; }

  // Gets the seconds that have passed between the last two advances.
  [[nodiscard]] f32 delta() const noexcept { return to_seconds(delta_); }

  // Gets how many seconds the tick is behind by. Time debt not yet accounted for by the tick rate.
  [[nodiscard]] f32 lag() const noexcept { return to_seconds(lag_); }

  // Gets how far into the next tick the last advance was.
  [[nodiscard]] f32 alpha() const noexcept { return f32(lag_.count()) / f32(period_.count()); }

  // Gets the tick rate in hertz.
  [[nodiscard]] constexpr u32 rate() const noexcept { return to_hertz(period_); }

  // Gets how many seconds pass between ticks.
  [[nodiscard]] constexpr f32 period() const noexcept { return to_seconds(period_); }

  void set_rate(u32 hertz) noexcept
  {
    PRECONDITION(hertz > 0);
    period_ = from_hertz(hertz);
  }

  void set_tick(i64 tick) noexcept
  {
    PRECONDITION(tick >= 0);
    tick_ = tick;
    reset();
  }

  void reset() noexcept
  {
    delta_ = Duration::zero();
    lag_   = Duration::zero();
    prior_ = std::chrono::steady_clock::now();
  }

private:

  [[nodiscard]] static constexpr f32 to_seconds(Duration duration) noexcept
  {
    return std::chrono::duration_cast<std::chrono::duration<f32>>(duration).count();
  }

  [[nodiscard]] static constexpr Duration from_hertz(u32 rate) noexcept
  {
    PRECONDITION(rate > 0);
    PRECONDITION(rate <= 1'000'000'000);
    return Duration(1'000'000'000 / rate);
  }

  [[nodiscard]] static constexpr u32 to_hertz(Duration period) noexcept
  {
    return 1'000'000'000 / period.count();
  }

  i64       tick_;   // how many periods elapsed
  Duration  period_; // period should not change after construction unless via `set_rate`
  Duration  delta_;
  Duration  lag_;
  TimePoint prior_; // time of the last advance
};


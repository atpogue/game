#pragma once
#include "engine/core/types.hh"

using hertz = u64;
using nanoseconds = u64;
using seconds = u64;

// hertz to nanoseconds
[[nodiscard]] constexpr nanoseconds from_hertz(hertz time) noexcept { return 1'000'000'000 / time; }

// nanoseconds to seconds
[[nodiscard]] constexpr seconds to_seconds(nanoseconds time) noexcept { return time / 1'000'000'000; }

struct TimeStep {

    constexpr explicit TimeStep(u64 size)
        : size_{size}, lag_{0u}, age_{0u}
    {
        // allow a step size of 0?
    }

    // Returns how many time steps to run this frame.
    [[nodiscard]] u64 advance(u64 dt) noexcept {
        if (size_ == 0u) return 0u;
        lag_ += dt;
        u64 steps = lag_ / size_;
        lag_ -= size_ * steps;
        age_ += steps;
        return steps;
    }

    [[nodiscard]] u64 lag() const noexcept { return lag_; }
    [[nodiscard]] u64 age() const noexcept { return age_; }
    [[nodiscard]] constexpr u64 size() const noexcept { return size_; }

    void reset(u64 size) { size_ = size; lag_ = 0.f; age_ = 0u; }

    void reset() { lag_ = 0.f; age_ = 0u; }

private:

    // made with nanoseconds in mind by the unit is arbitrary

    u64 size_; // this should not change after construction unless reseting
    u64 lag_;
    u64 age_;

};


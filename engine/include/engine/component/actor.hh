#pragma once
#include "engine/action.hh"
#include <memory>
#include <vector>

// TODO: polling of action results
// TODO: action priority?

struct Actor {

    Actor(u8 size);

    Actor(Actor &&) noexcept = default;
    Actor &operator=(Actor &&) noexcept = default;

    Actor(const Actor &) = delete;
    Actor &operator=(const Actor &) = delete;

    // advances all actions forwards one simulation step
    void step(Registry &r, Entity e);

    // can be rejected if action is incompatible or if action queue is full
    bool try_submit(std::unique_ptr<Action> action);

    // get how many actions the actor can have simultaneously
    [[nodiscard]] constexpr u8 capacity() const { return buffer_.capacity(); }

    constexpr void capacity(u8 size) { buffer_.reserve(size); }

    // returns the number of queued actions, 0 if idle
    [[nodiscard]] constexpr u8 action_count() const { return buffer_.size(); }

private:

    std::vector<std::unique_ptr<Action>> buffer_;

};


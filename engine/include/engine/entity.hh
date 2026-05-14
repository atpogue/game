#pragma once
#include "engine/core/handle.hh"

struct Entity {

    [[nodiscard]] static constexpr Entity null() { return Entity(); }

    Entity() = default;

    Entity(const Entity &) = default;
    Entity &operator=(const Entity &) = default;

    Entity(Handle<void> handle) : handle_(handle) {}
    Entity &operator=(const Handle<void> &handle) { handle_ = handle; return *this; }

    auto operator<=>(const Entity &) const = default;

    constexpr explicit operator Handle<void>() { return handle_; }

private:

    Handle<void> handle_;

};


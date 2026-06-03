#pragma once
#include "engine/core/handle.hh"

struct Entity {

    Entity() = default;

    constexpr Entity(const Entity &) noexcept = default;
    constexpr Entity &operator=(const Entity &) noexcept = default;

    constexpr Entity(Handle<> handle) noexcept : handle_(handle) {}
    constexpr Entity &operator=(const Handle<> &handle) noexcept { handle_ = handle; return *this; }

    [[nodiscard]] constexpr auto operator<=>(const Entity &) const noexcept = default;

    template <typename Tag=void>
    [[nodiscard]] constexpr Handle<Tag> to_handle() const noexcept {
        return handle_.with_tag<Tag>();
    }

private:

    Handle<> handle_;

};

constexpr Entity nil_entity;


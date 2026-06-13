#pragma once
#include "core/handle.hh"

struct Entity
{
  Entity()                                            = default;
  constexpr Entity(Entity const&) noexcept            = default;
  constexpr Entity& operator=(Entity const&) noexcept = default;

  constexpr Entity(Handle<> handle) noexcept : handle_(handle) {}

  constexpr Entity& operator=(Handle<> const& handle) noexcept
  {
    handle_ = handle;
    return *this;
  }

  [[nodiscard]] constexpr auto operator<=>(Entity const&) const noexcept = default;

  template <typename Tag = void>
  [[nodiscard]] constexpr Handle<Tag> to_handle() const noexcept
  {
    return handle_.with_tag<Tag>();
  }

private:

  Handle<> handle_;
};

constexpr Entity nil_entity;

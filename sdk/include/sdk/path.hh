#pragma once
#include "core/panic.hh"
#include "sdk/types.hh"
#include <initializer_list>
#include <span>
#include <string>
#include <vector>

struct LuaPath;

std::string describe(LuaKey const& key);
std::string describe(LuaPath const& path);
std::string describe(std::span<LuaKey const> path);

struct LuaPath
{
  LuaPath()                              = default;
  LuaPath(LuaPath const&)                = default;
  LuaPath(LuaPath&&) noexcept            = default;
  LuaPath& operator=(LuaPath const&)     = default;
  LuaPath& operator=(LuaPath&&) noexcept = default;
  ~LuaPath() noexcept                    = default;

  constexpr LuaPath(std::initializer_list<LuaKey> keys) : keys_(keys) {}

  constexpr LuaPath(std::span<LuaKey const> keys) : keys_(keys.begin(), keys.end()) {}

  constexpr LuaKey const& operator[](size_t i) const { return keys_[i]; }

  LuaPath& operator+=(LuaPath const& rhs)
  {
    keys_.insert(keys_.end(), rhs.keys_.begin(), rhs.keys_.end());
    return *this;
  }

  friend LuaPath operator+(LuaPath lhs, LuaPath const& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  LuaPath& operator+=(LuaKey const& key)
  {
    keys_.push_back(key);
    return *this;
  }

  friend LuaPath operator+(LuaPath lhs, LuaKey const& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  constexpr size_t size() const { return keys_.size(); }

  constexpr bool empty() const { return keys_.empty(); }

  constexpr auto begin() const { return keys_.begin(); }

  constexpr auto end() const { return keys_.end(); }

  constexpr std::span<LuaKey const> span() const { return keys_; }

  constexpr std::span<LuaKey const> span(size_t length) const
  {
    PRECONDITION(length < keys_.size());
    return { keys_.begin(), keys_.begin() + long(length) };
  }

private:
  std::vector<LuaKey> keys_;
};


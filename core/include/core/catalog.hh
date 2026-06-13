#pragma once
#include "core/panic.hh"
#include "core/string.hh"
#include <vector>

template <typename Type>
concept Named = requires (Type const& type) {
  { type.name } -> std::convertible_to<std::string_view>;
};

// An append-only, lookup-by-name set of elements with ID's for quick runtime
// reference.
template <Named Type>
struct Catalog
{
  Catalog(u32 capacity) : Catalog() { reserve(capacity); }

  Catalog()                              = default;
  Catalog(Catalog&&) noexcept            = default;
  Catalog& operator=(Catalog&&) noexcept = default;
  Catalog& operator=(Catalog const&)     = delete;
  ~Catalog() noexcept                    = default;

  // Assumes: name doesn't already exist, maximum size not reached.
  template <typename... Args>
  requires std::constructible_from<Type, Args...>
  u32 emplace(Args&&... args)
  {
    // throw exception here instead of panic??
    ASSERT(data_.size() < UINT32_MAX, "maximum size reached");
    u32 const id    = data_.size();
    Type&     item  = data_.emplace_back(std::forward<Args>(args)...);
    auto [_, added] = lookup_.emplace(item.name, id);
    PRECONDITION(added, "name must not be assigned to more than one element");
    return id;
  }

  void clear() noexcept
  {
    data_.clear();
    lookup_.clear();
  }

  [[nodiscard]] Type const& operator[](u32 id) const noexcept
  {
    DEBUG_ASSERT(has(id));
    return data_[id];
  }

  [[nodiscard]] Type& operator[](u32 id) noexcept
  {
    DEBUG_ASSERT(has(id));
    return data_[id];
  }

  // Returns null if the element doesn't exist.
  [[nodiscard]] Type const* get(std::string_view name) const
  {
    auto id = find(name);
    return id != nil ? &data_[id] : nullptr;
  }

  // Returns null if the element doesn't exist.
  [[nodiscard]] Type* get(std::string_view name)
  {
    auto id = find(name);
    return id != nil ? &data_[id] : nullptr;
  }

  // Returns the ID (index) of the element or nil if not found.
  [[nodiscard]] u32 find(std::string_view name) const
  {
    auto it = lookup_.find(name);
    return it != lookup_.end() ? it->second : nil;
  }

  bool has(u32 id) const noexcept { return id < data_.size(); }

  [[nodiscard]] size_t size() const noexcept { return data_.size(); }

  void reserve(u32 n)
  {
    data_.reserve(n);
    lookup_.reserve(n);
  }

  [[nodiscard]] Catalog copy() const requires std::is_copy_constructible_v<Type> { return *this; }

  using iterator       = std::vector<Type>::iterator;
  using const_iterator = std::vector<Type>::const_iterator;

  [[nodiscard]] const_iterator cbegin() const noexcept { return data_.begin(); }

  [[nodiscard]] iterator begin() noexcept { return data_.begin(); }

  [[nodiscard]] const_iterator begin() const noexcept { return cbegin(); }

  [[nodiscard]] const_iterator cend() const noexcept { return data_.end(); }

  [[nodiscard]] iterator end() noexcept { return data_.end(); }

  [[nodiscard]] const_iterator end() const noexcept { return cend(); }

private:

  Catalog(Catalog const&) requires std::is_copy_constructible_v<Type>
  = default;

  std::vector<Type> data_;
  StringMap<u32>    lookup_;
};

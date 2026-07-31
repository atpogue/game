#pragma once

// TODO: ability to query all entity with a series of components; regstry.query<Ts...>()
// TODO: ability to sort component stores in relation to each other so that running a query on them
// in a hot path is more performant: registry.sort<Ts...>()

#include "core/panic.hh"
#include "core/slot-map.hh"
#include "core/sparse-set.hh"
#include "core/type-list.hh"
#include <span>
#include <tuple>

template <typename Key, typename List>
struct BasicRegistry;

template <typename Key, typename... Types>
struct BasicRegistry<Key, TypeList<Types...>>
{ //////////////////////////////////////////////////////////////////////////////

  BasicRegistry(u32 limit = UINT32_MAX) : keys_(limit), stores_() {}

  BasicRegistry(BasicRegistry&&)                 = default;
  BasicRegistry& operator=(BasicRegistry&&)      = default;
  BasicRegistry& operator=(BasicRegistry const&) = delete;

  bool valid(Handle<Key> handle) const { return keys_.has(handle); }

  Key& operator[](Handle<Key> handle)
  {
    DEBUG_ASSERT(valid(handle));
    return keys_[handle];
  }

  Key const& operator[](Handle<Key> handle) const
  {
    DEBUG_ASSERT(valid(handle));
    return keys_[handle];
  }

  template <typename T, typename... Args>
  requires std::constructible_from<T, Args...>
  T& emplace(Handle<Key> handle, Args&&... args)
  {
    PRECONDITION(valid(handle));
    return store<T>().emplace(handle.index, std::forward<Args>(args)...);
  }

  template <typename T>
  void erase(Handle<Key> handle)
  {
    PRECONDITION(valid(handle));
    store<T>().erase(handle.index);
  }

  template <typename T>
  bool try_erase(Handle<Key> handle)
  {
    auto& s = store<T>();
    if (!s.has(handle.index)) return false;
    s.erase(handle.index);
    return true;
  }

  void destroy(Handle<Key> handle)
  {
    PRECONDITION(valid(handle));
    (try_erase<Types>(handle), ...);
    keys_.erase(handle);
  }

  bool try_destroy(Handle<Key> handle)
  {
    if (!valid(handle)) return false;
    destroy(handle);
    return true;
  }

  template <typename T>
  [[nodiscard]] T const& get(Handle<Key> handle) const
  {
    DEBUG_ASSERT(valid(handle));
    return store<T>()[handle.index];
  }

  template <typename T>
  [[nodiscard]] T& get(Handle<Key> handle)
  {
    DEBUG_ASSERT(valid(handle));
    return store<T>()[handle.index];
  }

  template <typename T>
  [[nodiscard]] T const* try_get(Handle<Key> handle) const
  {
    return store<T>().try_get(handle.index);
  }

  template <typename T>
  [[nodiscard]] T* try_get(Handle<Key> handle)
  {
    return store<T>().try_get(handle.index);
  }

  template <typename T>
  [[nodiscard]] bool has(Handle<Key> handle) const
  {
    return store<T>().has(handle.index);
  }

  template <typename... Ts>
  [[nodiscard]] bool all(Handle<Key> handle) const
  {
    return (store<Ts>().has(handle.index) && ...);
  }

  template <typename... Ts>
  [[nodiscard]] bool any(Handle<Key> handle) const
  {
    return (store<Ts>().has(handle.index) || ...);
  }

  template <typename... Args>
  requires std::constructible_from<Key, Args...>
  [[nodiscard]] Handle<Key> create(Args&&... args)
  {
    return keys_.emplace(std::forward<Args>(args)...);
  }

  [[nodiscard]] constexpr u32 capacity() const { return keys_.capacity(); }

  // Gets the number of live entities;
  [[nodiscard]] u32 size() const { return keys_.size(); }

  [[nodiscard]] constexpr u32 limit() const { return keys_.limit(); }

  void clear()
  {
    std::apply([](auto&... stores) { (stores.clear(), ...); }, stores_);
    keys_.clear();
  }

  using const_iterator = SlotMap<Key>::const_iterator;
  using iterator       = SlotMap<Key>::iterator;

  [[nodiscard]] iterator begin() { return keys_.begin(); }

  [[nodiscard]] iterator end() { return keys_.end(); }

  [[nodiscard]] const_iterator cbegin() const { return keys_.cbegin(); }

  [[nodiscard]] const_iterator cend() const { return keys_.cend(); }

  [[nodiscard]] const_iterator begin() const { return cbegin(); }

  [[nodiscard]] const_iterator end() const { return cend(); }

  // Explicit copy to prevent unintended implicit copy construction.
  // Won't work if any of the component types are not copy constructible.
  [[nodiscard]] BasicRegistry copy() const { return *this; }

  template <typename T>
  constexpr std::span<T> each()
  {
    return store<T>().values();
  }

  template <typename T>
  constexpr std::span<T const> each() const
  {
    return store<T>().values();
  }

private:

  BasicRegistry(BasicRegistry const&) = default;

  template <typename T>
  constexpr SparseSet<T>& store()
  {
    return std::get<SparseSet<T>>(stores_);
  }

  template <typename T>
  constexpr SparseSet<T> const& store() const
  {
    return std::get<SparseSet<T>>(stores_);
  }

  SlotMap<Key>                    keys_;
  std::tuple<SparseSet<Types>...> stores_;

}; /////////////////////////////////////////////////////////////////////////////

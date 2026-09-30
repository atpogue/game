#pragma once
#include "core/indexed-map.hh"
#include "core/panic.hh"
#include "core/type-list.hh"
#include <span>
#include <string_view>
#include <tuple>

// Goal: append-only, patchable, stable index, serializable
template <typename List>
struct BasicCatalog;

template <typename... Types>
struct BasicCatalog<TypeList<Types...>>
{
  BasicCatalog()                               = default;
  BasicCatalog(BasicCatalog&&)                 = default;
  BasicCatalog& operator=(BasicCatalog&&)      = default;
  BasicCatalog& operator=(BasicCatalog const&) = delete;

  template <typename T>
  [[nodiscard]] bool valid(Handle<T> handle) const
  {
    return handle.generation == generation_ && handle.index < store_of<T>().size();
  }

  template <typename T>
  [[nodiscard]] Handle<T> find(std::string_view label) const
  {
    return { store_of<T>().find(label), generation_ };
  }

  template <typename T>
  [[nodiscard]] std::string_view label(Handle<T> handle) const
  {
    DEBUG_ASSERT(valid(handle));
    return store_of<T>().key(handle.index);
  }

  template <typename T>
  [[nodiscard]] T& operator[](Handle<T> handle)
  {
    DEBUG_ASSERT(valid(handle));
    return store_of<T>()[handle.index];
  }

  template <typename T>
  [[nodiscard]] T const& operator[](Handle<T> handle) const
  {
    DEBUG_ASSERT(valid(handle));
    return store_of<T>()[handle.index];
  }

  template <typename T, typename... Args>
  requires std::constructible_from<T, Args...>
  Handle<T> emplace(std::string_view label, Args&&... args)
  {
    return { store_of<T>().emplace(label, std::forward<Args>(args)...), generation_ };
  }

  template <typename T>
  [[nodiscard]] T const* try_get(Handle<T> handle) const
  {
    if (!valid(handle)) return nullptr;
    return &store_of<T>()[handle.index];
  }

  template <typename T>
  [[nodiscard]] T* try_get(Handle<T> handle)
  {
    if (!valid(handle)) return nullptr;
    return &store_of<T>()[handle.index];
  }

  void clear()
  {
    std::apply([](auto&... stores) { (stores.clear(), ...); }, stores_);
  }

  template <typename T>
  [[nodiscard]] u32 count() const
  {
    return store_of<T>().size();
  }

  template <typename T>
  [[nodiscard]] std::span<T> each()
  {
    return store_of<T>().values();
  }

  template <typename T>
  [[nodiscard]] std::span<T const> each() const
  {
    return store_of<T>().values();
  }

  // Explicit copy to prevent unintended implicit copy construction.
  // Won't work if any of the component types are not copy constructible.
  [[nodiscard]] BasicCatalog copy() const { return *this; }

private:

  template <typename T>
  constexpr IndexedMap<std::string, T>& store_of()
  {
    return std::get<IndexedMap<std::string, T>>(stores_);
  }

  template <typename T>
  constexpr IndexedMap<std::string, T> const& store_of() const
  {
    return std::get<IndexedMap<std::string, T>>(stores_);
  }

  BasicCatalog(BasicCatalog const&) = default;

  u32                                           generation_ = 0u;
  std::tuple<IndexedMap<std::string, Types>...> stores_;
};


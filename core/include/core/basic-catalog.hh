#pragma once
#include "core/indexed-map.hh"
#include "core/panic.hh"
#include "core/type-list.hh"
#include <span>
#include <string_view>
#include <tuple>

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
  [[nodiscard]] bool valid(Token<T> token) const
  {
    return token.value < store_of<T>().size();
  }

  template <typename T>
  [[nodiscard]] Token<T> find(std::string_view label) const
  {
    return { store_of<T>().find(label) };
  }

  template <typename T>
  [[nodiscard]] std::string_view label(Token<T> token) const
  {
    DEBUG_ASSERT(valid(token));
    return store_of<T>().key(token.value);
  }

  template <typename T>
  [[nodiscard]] T& operator[](Token<T> token)
  {
    DEBUG_ASSERT(valid(token));
    return store_of<T>()[token.value];
  }

  template <typename T>
  [[nodiscard]] T const& operator[](Token<T> token) const
  {
    DEBUG_ASSERT(valid(token));
    return store_of<T>()[token.value];
  }

  template <typename T, typename... Args>
  requires std::constructible_from<T, Args...>
  Token<T> emplace(std::string_view label, Args&&... args)
  {
    return { store_of<T>().emplace(label, std::forward<Args>(args)...) };
  }

  template <typename T>
  [[nodiscard]] T const* try_get(std::string_view label) const
  {
    return store_of<T>().try_get(label);
  }

  template <typename T>
  [[nodiscard]] T* try_get(std::string_view label)
  {
    return store_of<T>().try_get(label);
  }

  void clear()
  {
    std::apply([](auto&... stores) { (stores.clear(), ...); }, stores_);
  }

  template <typename T>
  [[nodiscard]] constexpr std::span<T> each()
  {
    return store_of<T>().values();
  }

  template <typename T>
  [[nodiscard]] constexpr std::span<T const> each() const
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

  std::tuple<IndexedMap<std::string, Types>...> stores_;
};


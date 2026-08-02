#pragma once
#include "core/basic-catalog.hh"
#include "types.hh"
#include "world/terrain.hh"

using Definitions = TypeList<Terrain>;

using Catalog = BasicCatalog<Definitions>;

[[nodiscard]] CatalogReader access_catalog(ConstContext ctx);
[[nodiscard]] CatalogWriter access_catalog(LoadContext ctx);

// A subset of the definitions catalog API that simulation systems can use.
template <AccessPolicy Access>
struct CatalogView
{
  using Source = std::conditional_t<Access, Catalog, Catalog const>;

  CatalogView(Source& src) noexcept : src_(src) {}

  CatalogView(CatalogView&&) noexcept      = default;
  CatalogView(CatalogView const&) noexcept = default;
  ~CatalogView() noexcept                  = default;

  template <typename T>
  [[nodiscard]] bool valid(Token<T> token) const
  {
    return src_.valid(token);
  }

  template <typename T>
  [[nodiscard]] Token<T> find(std::string_view label) const
  {
    return src_.template find<T>(label);
  }

  template <typename T>
  [[nodiscard]] std::string_view label(Token<T> token) const
  {
    return src_.label(token);
  }

  template <typename T>
  [[nodiscard]] auto& operator[](Token<T> token) const
  {
    return src_[token];
  }

  template <typename T>
  [[nodiscard]] auto* try_get(std::string_view label) const
  {
    return src_.template try_get<T>(label);
  }

  template <typename T, typename... Args>
  requires (Access == Write) && std::constructible_from<T, Args...>
  Token<T> emplace(std::string_view label, Args&&... args) const
  {
    return src_.template emplace<T, Args...>(label, std::forward<Args>(args)...);
  }

  template <typename T>
  requires (Access == Write)
  void erase(Token<T> id) const
  {
    src_.template erase<T>(id);
  }

  template <typename T>
  constexpr auto each() const
  {
    return src_.template each<T>();
  }

private:

  Source& src_;
};

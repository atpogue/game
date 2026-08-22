#pragma once
#include "core/basic-registry.hh"
#include "game/component/pose.hh"
#include "game/types.hh"

using Components = TypeList<Pose>;

using Registry = BasicRegistry<Entity, Components>;

[[nodiscard]] RegistryReader access_registry(ConstContext ctx);
[[nodiscard]] RegistryWriter access_registry(Context ctx);

// A subset of the component registry API that simulation systems can use.
template <AccessFlag Access>
struct RegistryView
{
  using Source = std::conditional_t<Access == Write, Registry, Registry const>;

  RegistryView(Source& src) noexcept : src_(src) {}

  RegistryView(RegistryView&&) noexcept      = default;
  RegistryView(RegistryView const&) noexcept = default;
  ~RegistryView() noexcept                   = default;

  [[nodiscard]] bool valid(Handle<Entity> handle) const { return src_.valid(handle); }

  [[nodiscard]] Entity id(Handle<Entity> handle) const { return src_[handle]; }

  template <typename T>
  [[nodiscard]] bool has(Handle<Entity> handle) const
  {
    return src_.template has<T>(handle);
  }

  template <typename... Ts>
  [[nodiscard]] bool all(Handle<Entity> handle) const
  {
    return src_.template all<Ts...>(handle);
  }

  template <typename... Ts>
  [[nodiscard]] bool any(Handle<Entity> handle) const
  {
    return src_.template any<Ts...>(handle);
  }

  template <typename T>
  [[nodiscard]] auto& get(Handle<Entity> handle) const
  {
    return src_.template get<T>(handle);
  }

  template <typename T>
  [[nodiscard]] auto* try_get(Handle<Entity> handle) const
  {
    return src_.template try_get<T>(handle);
  }

  template <typename T, typename... Args>
  requires (Access == Write) && std::constructible_from<T, Args...>
  T& emplace(Handle<Entity> handle, Args&&... args) const
  {
    return src_.template emplace<T, Args...>(handle, std::forward<Args>(args)...);
  }

  template <typename T>
  requires (Access == Write)
  void erase(Handle<Entity> handle) const
  {
    src_.template erase<T>(handle);
  }

  template <typename T>
  requires (Access == Write)
  bool try_erase(Handle<Entity> handle) const
  {
    return src_.template try_erase<T>(handle);
  }

  template <typename T>
  [[nodiscard]] constexpr auto each() const
  {
    return src_.template each<T>();
  }

private:

  Source& src_;
};

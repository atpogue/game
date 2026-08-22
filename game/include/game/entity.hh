#pragma once

// TODO: `EntityBuilder` that does not register its ID as active until all its components are
// constructed and the user calls `builder.commit()`. If the builder goes out of scope, the
// ID is released but not retired (may be used again). If multi-threading is ever implemented, this
// would mean the entity could not be polled by other systems mid-construction.

// TODO: Lua defined entity prefabs.

// TODO: Entity destruction and a way to retire an entity ID. Is abscence of ID in persistent
// storage enough or are tombstones useful?

// TODO: Persistence and lazy loading; Entities can be loaded in session memory (the
// registry) or stored on disk, just because an entity does not exist in the registry doesn't mean
// it is dead/retired, it may not be loaded. Use SQLite? LevelDB?

#include "game/context.hh"
#include "game/registry.hh"

[[nodiscard]] EntityWriter create_entity(Context ctx);

[[nodiscard]] EntityReader access_entity(ConstContext ctx, Handle<Entity> e);
[[nodiscard]] EntityWriter access_entity(Context ctx, Handle<Entity> e);

template <AccessFlag Access>
struct EntityView
{
  // Not sure if this class is necessary. The same function can be achieved with a handle and a
  // registry view.

  using Source = std::conditional_t<Access == Write, Registry, Registry const>;

  EntityView(Source& src, Handle<Entity> e) noexcept : src_(src), handle_(e)
  {
    INVARIANT(handle_);
    INVARIANT(src_.valid(handle_));
  }

  EntityView(EntityView&&) noexcept      = default;
  EntityView(EntityView const&) noexcept = default;
  ~EntityView() noexcept                 = default;

  template <typename T>
  [[nodiscard]] bool has() const
  {
    return src_.template has<T>(handle_);
  }

  template <typename... Ts>
  [[nodiscard]] bool all() const
  {
    return src_.template all<Ts...>(handle_);
  }

  template <typename... Ts>
  [[nodiscard]] bool any() const
  {
    return src_.template any<Ts...>(handle_);
  }

  template <typename T>
  [[nodiscard]] auto& get() const
  {
    return src_.template get<T>(handle_);
  }

  template <typename T>
  [[nodiscard]] auto* try_get() const
  {
    return src_.template try_get<T>(handle_);
  }

  template <typename T, typename... Args>
  requires (Access == Write) && std::constructible_from<T, Args...>
  T& emplace(Args&&... args) const
  {
    return src_.template emplace<T>(handle_, std::forward<Args>(args)...);
  }

  template <typename T>
  requires (Access == Write)
  void erase() const
  {
    src_.template erase<T>(handle_);
  }

  [[nodiscard]] Handle<Entity> handle() const { return handle_; }

  [[nodiscard]] operator Handle<Entity>() const { return handle(); }

  [[nodiscard]] Entity id() const { return src_[handle_]; }

  [[nodiscard]] operator Entity() const { return id(); }

private:

  Source&        src_;
  Handle<Entity> handle_; // should never be null, assumed to be alive
};


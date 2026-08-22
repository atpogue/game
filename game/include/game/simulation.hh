#pragma once
#include "core/error.hh"
#include "core/panic.hh"
#include "game/catalog.hh"
#include "game/chunk.hh"
#include "game/context.hh"
#include "game/registry.hh"
#include "game/types.hh"
#include <unordered_map>

struct Simulation
{
  Simulation()                                 = default;
  Simulation(Simulation&&) noexcept            = default;
  Simulation& operator=(Simulation&&) noexcept = default;
  Simulation& operator=(Simulation const&)     = delete;

  inline Catalog& catalog() { return codex_; }

  inline Catalog const& catalog() const { return codex_; }

  inline Registry& registry() { return registry_; }

  inline Registry const& registry() const { return registry_; }

  inline Chunk& scene() { return scene_; }

  inline Chunk const& scene() const { return scene_; }

  [[nodiscard]] Handle<Entity> find(Entity e) const
  {
    auto it = lookup_.find(e);
    return it != lookup_.end() ? it->second : Handle<Entity>::null();
  }

  Handle<Entity> create()
  {
    auto id           = Entity{ entity_count_++ };
    auto handle       = registry_.create(id);
    auto [_, success] = lookup_.emplace(id, handle);
    INVARIANT(success);
    return handle;
  }

  Context step() { return *this; }

  Result<LoadContext> load() { return LoadContext(*this); }

  void quit() {}

  [[nodiscard]] ConstContext context() const { return *this; }

  // Explciit copy to prevent unintended and expensive implicit copies.
  inline Simulation clone() const { return *this; }

private:

  Simulation(Simulation const&) : codex_(), registry_(), scene_() {}

  Catalog  codex_;
  Registry registry_; // entity data
  Chunk    scene_;

  // Hash map used because entity ID is supposed to be sparse not dense.
  std::unordered_map<Entity, Handle<Entity>> lookup_;

  // Total number of entities including those not loaded (stored on disk).
  u64 entity_count_;
};

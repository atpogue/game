#pragma once
#include "core/basic-catalog.hh"
#include "core/basic-registry.hh"
#include "game/chunk.hh"
#include "game/component/pose.hh"
#include "game/content/terrain.hh"
#include "game/entity.hh"
#include <unordered_map>

namespace Game {
  using Definitions = TypeList<Terrain>;
  using Catalog     = BasicCatalog<Definitions>;

  using Components = TypeList<Pose>;
  using Registry   = BasicRegistry<Entity, Components>;

  struct World
  {
    Catalog  content;     // immutable after loading
    Registry entities;    // entity data
    Chunk    environment; // placeholder for a chunked world

    [[nodiscard]] Handle<Entity> find(Entity e) const;

    [[nodiscard]] Handle<Entity> create();

    void advance();

  private:

    // Total number of entities including those not loaded.
    u64 entity_counter_ = 0u;

    // Hash map used because entity IDs are sparse, not dense.
    std::unordered_map<Entity, Handle<Entity>> lookup_;
  };
}

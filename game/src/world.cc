#include "game/world.hh"
#include "core/panic.hh"

namespace Game {
  Handle<Entity> World::create()
  {
    auto const           id     = Entity{ entity_counter_++ };
    Handle<Entity> const handle = entities.create(id);
    auto [_, success]           = lookup_.emplace(id, handle);
    INVARIANT(success);
    return handle;
  }

  Handle<Entity> World::find(Entity e) const
  {
    auto it = lookup_.find(e);
    return it != lookup_.end() ? it->second : Handle<Entity>::null();
  }

  void World::advance() {}
}

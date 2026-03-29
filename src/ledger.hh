#pragma once
#include "entity.hh"

namespace ledger {

    Signature signature(Entity e);
    bool status(Entity e);
    bool has(Entity e, Component c);
    size_t index(Entity e, Component c);
    Index index(Entity e);
    bool assign(Entity e, Component c, size_t i);
    Entity create();
    void destroy(Entity e);

} // namespace ledger


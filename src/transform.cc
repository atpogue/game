#include "transform.hh"
#include "ledger.hh"
#include "sparse-map.hh"
#include <utility> // std::move

namespace {
    SparseMap<MoveComponent, Id> components;
}

MoveComponent *transforms::get(Entity e) {
    assert(!ledger::has(e, Component::Transform) || components.has(entity_id(e)));
    return components.get(entity_id(e));
}

MoveComponent *transforms::set(Entity e, MoveComponent &&c) {
    if (!ledger::status(e)) {
        assert(!"creating component for invalid entity");
        return nullptr;
    }
    ledger::sign(e, Component::Transform, true);
    auto result = &components.set(entity_id(e), std::move(c));
    assert(components.has(entity_id(e)));
    return result;
}

void transforms::destroy(Entity e) {
    if (!components.has(entity_id(e))) return;
    assert(ledger::has(e, Component::Transform) && "entity signature is not truthful");
    ledger::sign(e, Component::Transform, false);
    components.erase(entity_id(e));
}


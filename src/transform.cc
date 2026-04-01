#include "transform.hh"
#include "ledger.hh"
#include <utility> // std::move
#include <flat_map>

namespace {
    std::flat_map<Entity, MoveComponent> components;
}

MoveComponent *transforms::get(Entity e) {
    auto it = components.find(e);
    if (it == components.end()) return nullptr;
    return &it->second;
}

MoveComponent *transforms::set(Entity e, MoveComponent &&c) {
    if (!ledger::status(e)) {
        assert(!"creating component for invalid entity");
        return nullptr;
    }
    ledger::sign(e, Component::Transform, true);
    auto [it, _] = components.emplace(e, std::move(c));
    return &it->second;
}

void transforms::destroy(Entity e) {
    if (!components.contains(e)) return;
    assert(ledger::has(e, Component::Transform) && "entity signature is not truthful");
    ledger::sign(e, Component::Transform, false);
    components.erase(e);
}


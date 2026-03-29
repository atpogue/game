#include "movement.hh"
#include "ledger.hh"
#include <utility> // std::move
#include <vector>

namespace {
    std::vector<MoveComponent> components;
}

MoveComponent *movement::get(Entity e) {
    if (!ledger::status(e)) {
        // ledger::error("invalid entity")
        return nullptr;
    }
    auto i = ledger::index(e, COMPONENT_MOVE);
    if (i < components.size()) {
        // ledger::error("invalid index");
        return nullptr;
    }
    return &components[i];
}

void movement::set(Entity e, MoveComponent &&c) {
    auto i = components.size();
    ledger::assign(e, COMPONENT_MOVE, i);
    components.emplace_back(std::move(c));
}


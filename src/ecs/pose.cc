#include "ecs/pose.hh"
#include "ecs/ledger.hh"
#include "core/sparse-map.hh"
#include <utility> // std::move

namespace {
    SparseMap<Pose> components;
}

Pose *poses::get(Entity e) {
    assert(!ledger::has(e, Flag::Pose) || components.has(e.index));
    return components.get(e.index);
}

Pose *poses::set(Entity e, Pose &&pose) {
    if (!ledger::status(e)) {
        assert(!"creating component for invalid entity");
        return nullptr;
    }
    ledger::sign(e, Flag::Pose, true);
    Pose &c = components.emplace(e.index, std::move(pose));
    assert(components.has(e.index));
    return &c;
}

void poses::destroy(Entity e) {
    if (!components.has(e.index)) return;
    assert(ledger::has(e, Flag::Pose) && "entity signature is not truthful");
    ledger::sign(e, Flag::Pose, false);
    components.erase(e.index);
}


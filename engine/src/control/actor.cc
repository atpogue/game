#include "engine/control/actor.hh"
#include "engine/control/action.hh"
#include "engine/core/sparse-map.hh"
#include <cassert>
#include <memory>

// TODO: action priority?

namespace { ///////////////////////////////////////////////////////////////////////////////////
    using ActionQueue = std::vector<std::unique_ptr<Action>>;
    SparseMap<ActionQueue> components;

    ActionQueue *get(Entity e) {
        assert(!ledger::has(e, Flag::Actor) || components.has(e.index));
        return components.get(e.index);
    }
} /////////////////////////////////////////////////////////////////////////////////////////////

void actors::create(Entity e, unsigned short capacity) {
    if (components.has(e.index)) return;
    assert(!ledger::has(e, Flag::Actor) && "entity signature is not truthful");
    ledger::sign(e, Flag::Actor, true);
    auto &q = components.emplace(e.index);
    assert(components.has(e.index));
    q.reserve(capacity);
}

void actors::destroy(Entity e) {
    if (!components.has(e.index)) return;
    assert(ledger::has(e, Flag::Actor) && "entity signature is not truthful");
    ledger::sign(e, Flag::Actor, false);
    components.erase(e.index);
}

void actors::step() {
    for (auto &[i, queue] : components) {
        for (auto &action : queue) {
            assert(action && "action queue has null action");
            assert(!action->is_complete() && "action queue has completed action");
            action->step({i, ledger::generation(i)});
        }

        std::erase_if(queue, [](const auto &action) { return action->is_complete(); });
    }
}

unsigned short actors::busy(Entity e) {
    auto q = get(e);
    if (!q) return 0u;
    return q->size();
}

bool actors::act(Entity e, const Command &cmd) {
    ActionQueue *q = get(e);
    if (!q) return false;
    if (q->size() == q->capacity()) return false;
    q->emplace_back(make_action(e, cmd));
    return true;
}

unsigned short actors::capacity(Entity e) {
    auto q = get(e);
    if (!q) return 0u;
    return q->capacity();
}

void actors::capacity(Entity e, unsigned short cap) {
    auto q = get(e);
    if (!q) return;
    q->reserve(cap);
}


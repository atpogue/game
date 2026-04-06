#include "actor.hh"
#include "sparse-map.hh"
#include <cassert>

// TODO: action priority?
// TODO: culmulative log of last N actions

namespace { ///////////////////////////////////////////////////////////////////////////////////
    SparseMap<ActionQueue> components;

    ActionQueue *get(Entity e) {
        assert(!ledger::has(e, Flag::Actor) || components.has(e.index));
        return components.get(e.index);
    }

    bool translate(Entity e, ActionQueue &q, const Command &cmd) {
        if (q.size() == q.capacity()) return false;
        bool accepted = false;
        switch (cmd.type) {
        case Command::Type::Move:
            if (!ledger::has(e, Flag::Pose)) break;
            q.emplace_back(std::make_unique<MoveAction>(cmd.move.x, cmd.move.y), 1u);
            accepted = true;
            break;
        default:
            assert(!"Command type has no action translation");
            break;
        }
        return accepted;
    }
} /////////////////////////////////////////////////////////////////////////////////////////////

void actors::create(Entity e, unsigned short capacity) {
    if (components.has(e.index)) return;
    assert(!ledger::has(e, Flag::Actor) && "entity signature is not truthful");
    ledger::sign(e, Flag::Actor, true);
    auto &q = components.emplace(e.index, e.generation);
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
    for (auto &[i, q] : components) {
        for (auto &pending : q) {
            if (pending.age < pending.expiry
            &&  pending.action != nullptr
            &&  pending.action->status() == Action::Status::Busy)
            {
                pending.action->step({i, ledger::generation(i)});
            }
            pending.age++;
        }

        std::erase_if(q,
            [](const PendingAction &pending) {
                return  pending.age >= pending.expiry
                    || !pending.action
                    ||  pending.action->status() != Action::Status::Busy;
            }
        );
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
    return translate(e, *q, cmd);
}

std::vector<bool> actors::act(Entity e, const CommandQueue &cmds) {
    std::vector<bool> results;
    ActionQueue *q = get(e);
    if (!q) return results;
    results.reserve(cmds.size());
    for (auto &cmd : cmds) results.push_back(translate(e, *q, cmd));
    return results;
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


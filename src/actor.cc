#include "actor.hh"
#include "action.hh"
#include <cassert>
#include <flat_map>
#include <memory>

// TODO: action priority?
// TODO: culmulative log of last N actions

struct PendingAction {
    std::unique_ptr<Action> action;
    // if the action has not been made "hot" before the expiry is reached, it is marked invalid/stale
    unsigned expiry = 1u;    // the age at which the action expires if its still cold/pending
    unsigned age    = 0u;    // how long the action has been cold
    bool     active = false; // false if the action is still cold
};

struct Actor {
    std::vector<PendingAction> queue;
};

namespace { ///////////////////////////////////////////////////////////////////////////////////
    std::flat_map<Entity, Actor> components;

    Actor *get(Entity e) {
        auto it = components.find(e);
        if (it == components.end()) return nullptr;
        return &it->second;
    }

    bool translate(Entity e, Actor &actor, const Command &cmd) {
        bool accepted = false;
        switch (cmd.type) {
        case Command::Type::Move:
            if (ledger::has(e, Component::Transform)) {
                actor.queue.emplace_back(
                    std::make_unique<MoveAction>(cmd.move.x, cmd.move.y), 1u);
                accepted = true;
            } break;
        default:
            assert(!"Command type has no action translation");
            break;
        }
        return accepted;
    }
} /////////////////////////////////////////////////////////////////////////////////////////////

void actors::create(Entity e, unsigned short capacity) {
    if (components.contains(e)) return;
    assert(!ledger::has(e, Component::Actor) && "entity signature is not truthful");
    ledger::sign(e, Component::Actor, true);
    components[e].queue.reserve(capacity);
}

void actors::destroy(Entity e) {
    if (!components.contains(e)) return;
    assert(ledger::has(e, Component::Actor) && "entity signature is not truthful");
    ledger::sign(e, Component::Actor, false);
    components.erase(e);
}

void actors::step() {
    for (auto [e, actor] : components) {
        for (auto &entry : actor.queue) {
            if (entry.age < entry.expiry
            &&  entry.action != nullptr
            &&  entry.action->status() == Action::Status::Busy)
            {
                entry.action->step(e);
            }
            entry.age++;
        }

        std::erase_if(actor.queue,
            [](const PendingAction &entry) {
                return  entry.age >= entry.expiry
                    || !entry.action
                    ||  entry.action->status() != Action::Status::Busy;
            }
        );
    }
}

unsigned short actors::busy(Entity e) {
    auto actor = get(e);
    if (!actor) return 0u;
    return actor->queue.size();
}

std::vector<bool> actors::act(Entity e, const CommandQueue &cmds) {
    std::vector<bool> results;
    Actor *actor = get(e);
    if (!actor) return results;
    results.reserve(cmds.size());
    for (auto &cmd : cmds) results.push_back(translate(e, *actor, cmd));
    return results;
}

unsigned short actors::capacity(Entity e) {
    auto actor = get(e);
    if (!actor) return 0u;
    return actor->queue.capacity();
}

void actors::capacity(Entity e, unsigned short cap) {
    auto actor = get(e);
    if (!actor) return;
    actor->queue.reserve(cap);
}


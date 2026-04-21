#pragma once
#include "control/action.hh"
#include "control/command.hh"
#include "ecs/ledger.hh"
#include <vector>
#include <memory>

struct PendingAction {
    std::unique_ptr<Action> action;
    // if the action has not been made "hot" before the expiry is reached, it is marked invalid/stale
    unsigned expiry = 0u;    // the age at which the action expires if its still cold/pending
    unsigned age    = 0u;    // how long the action has been cold
    bool     active = false; // false if the action is still cold
};

using ActionQueue = std::vector<PendingAction>;

namespace actors {

    void create(Entity e, unsigned short capacity=4);
    void destroy(Entity e);

    void capacity(Entity e, unsigned short cap);
    unsigned short capacity(Entity e);

    // enqueue the action correlating to the given command
    // can be rejected if command is incompatible or if action queue is full
    bool act(Entity e, const Command &cmd);

    // returns the number of queued actions, 0 if idle
    unsigned short busy(Entity e);

    // progress the simulation forwards one fixed time step
    void step();

} // namespace actors


#pragma once
#include "control/command.hh"
#include "ecs/ledger.hh"

// TODO: polling of action/command results

namespace actors {

    void create(Entity e, unsigned short capacity=1);
    void destroy(Entity e);

    void capacity(Entity e, unsigned short cap);
    unsigned short capacity(Entity e);

    void disable(Entity e);
    void enable(Entity e);

    // enqueue the action correlating to the given command
    // can be rejected if command is incompatible or if action queue is full
    bool act(Entity e, const Command &cmd);

    // returns the number of queued actions, 0 if idle
    unsigned short busy(Entity e);

    void step();

} // namespace actors


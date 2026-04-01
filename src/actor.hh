#pragma once
#include "command.hh"
#include "ledger.hh"
#include <vector>

namespace actors {

    void create(Entity e, unsigned short capacity=4);
    void destroy(Entity e);

    void capacity(Entity e, unsigned short cap);
    unsigned short capacity(Entity e);

    // enqueue the action correlating to the given command
    // can be rejected if command is incompatible or if action queue is full
    std::vector<bool> act(Entity e, const CommandQueue &cmds);

    // returns the number of queued actions, 0 if idle
    unsigned short busy(Entity e);

    // progress the simulation forwards one fixed time step
    void step();

} // namespace actors


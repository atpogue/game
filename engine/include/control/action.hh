#pragma once
#include "ecs/ledger.hh"
#include "control/command.hh"

struct Action {

    virtual ~Action() = default;

    virtual bool is_complete() const = 0;

    // progress the simulation forwards one fixed time step
    // action steps run before simulation step
    virtual void step(Entity entity) = 0;

    // ask the action to complete or halt
    // can be rejected if doing so would leave the simulation in an invalid state
    virtual bool cancel() = 0;

};

Action *make_action(Entity e, const Command::Move &move);
Action *make_action(Entity e, const Command &cmd);


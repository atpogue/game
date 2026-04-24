#include "control/action.hh"
#include <cassert>

Action *make_action(Entity e, const Command &cmd) {
    switch (cmd.type) {
    case Command::Type::Move:
        return make_action(e, cmd.move);
        break;
    default:
        assert(!"Command type has no action translation");
        break;
    }
    return nullptr;
}


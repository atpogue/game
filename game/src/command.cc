#include "command.hh"
#include <cassert>

Command make_move_command(u32 id, f32 x, f32 y) {
    Command cmd;
    cmd.id = id;
    cmd.type = Command::Type::Move;
    cmd.move.x = x;
    cmd.move.y = y;
    return cmd;
}

Command make_path_command(u32 id, f32 x, f32 y) {
    Command cmd;
    cmd.id = id;
    cmd.type = Command::Type::Path;
    cmd.path.x = x;
    cmd.path.y = y;
    return cmd;
}

std::unique_ptr<Action> make_action(const Registry &reg, Entity e, const Command &cmd) {
    switch (cmd.type) {
    case Command::Type::Move: return make_action(reg, e, cmd.move);
    case Command::Type::Path: return make_action(reg, e, cmd.path);
    }
    return nullptr;
}


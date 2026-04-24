#include "control/command.hh"

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


#include "control/command.hh"

Command move_command(f32 x, f32 y) {
    Command cmd;
    cmd.type = Command::Type::Move;
    cmd.move.x = x;
    cmd.move.y = y;
    return cmd;
}

Command path_command(f32 x, f32 y) {
    Command cmd;
    cmd.type = Command::Type::PathTo;
    cmd.path.x = x;
    cmd.path.y = y;
    return cmd;
}


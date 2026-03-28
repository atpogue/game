#include "command.hh"

Command make_move_command(float x, float y) {
    Command cmd;
    cmd.type = Command::Type::Move;
    cmd.move.x = x;
    cmd.move.y = y;
    return cmd;
}

Command make_path_command(float x, float y) {
    Command cmd;
    cmd.type = Command::Type::PathTo;
    cmd.path.x = x;
    cmd.path.y = y;
    return cmd;
}


#include "action/move.hh"
#include "command.hh"
#include "component/pose.hh"
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

// Is this the right place to define this???
static bool try_submit_command(GameRegistry &r, Entity e, const Command::Move &cmd) {
    if (r.has<MoveAction>(e)) return false;
    if (!r.has<Pose>(e)) return false;
    r.emplace<MoveAction>(e, glm::vec2(cmd.x, cmd.y), 1.f);
    return true;
}

static bool try_submit_command(GameRegistry &, Entity, const Command::Path &) {
    return false;
}

bool try_submit_command(GameRegistry &r, Entity e, const Command &cmd) {
    switch (cmd.type) {
    case Command::Type::Move: return try_submit_command(r, e, cmd.move);
    case Command::Type::Path: return try_submit_command(r, e, cmd.path);
    }
    return false;
}


#include "command.hh"
#include "component/pose.hh"
#include "context.hh"
#include <glm/geometric.hpp>

Command make_move_command(u32 id, f32 x, f32 y) {
    return Command{
        id, Command::Type::Move,
        { .move = {x, y} }
    };
}

Command make_path_command(u32 id, f32 x, f32 y) {
    return Command{
        id, Command::Type::Path,
        { .path = {x, y} }
    };
}

static bool try_submit_command(Context ctx, Entity e, const Command::Move &cmd) {
    auto pose = ctx.entities.get<Pose>(e);
    if (!pose) return false;
    pose->position += glm::normalize(glm::vec2(cmd.x, cmd.y));
    return true;
}

static bool try_submit_command(Context, Entity, const Command::Path &) {
    return false;
}

bool try_submit_command(Context ctx, Entity e, const Command cmd) {
    if (!ctx.entities.is_alive(e)) return false;
    switch (cmd.type) {
    case Command::Type::Move: return try_submit_command(ctx, e, cmd.move);
    case Command::Type::Path: return try_submit_command(ctx, e, cmd.path);
    }
    return false;
}


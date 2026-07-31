#include "command.hh"
#include "component/pose.hh"
#include "context.hh"
#include <glm/geometric.hpp>

Command make_move_command(u32 id, f32 x, f32 y)
{
  return Command{id, Command::Type::Move, {.move = {x, y}}};
}

Command make_path_command(u32 id, f32 x, f32 y)
{
  return Command{id, Command::Type::Path, {.path = {x, y}}};
}

static bool try_submit_command(Context ctx, Handle<Entity> e, Command::Move const& cmd)
{
  auto pose = ctx.entities.try_get<Pose>(e);
  if (!pose) return false;
  pose->position += glm::normalize(glm::vec2(cmd.x, cmd.y));
  return true;
}

static bool try_submit_command(Context, Handle<Entity>, Command::Path const&) { return false; }

bool try_submit_command(Context ctx, Handle<Entity> e, Command const cmd)
{
  if (!ctx.entities.valid(e)) return false;
  switch (cmd.type) {
  case Command::Type::Move: return try_submit_command(ctx, e, cmd.move);
  case Command::Type::Path: return try_submit_command(ctx, e, cmd.path);
  }
  return false;
}

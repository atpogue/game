#include "action/move.hh"
#include "component/pose.hh"
#include "context.hh"
#include <glm/geometric.hpp>

ActionResult act(Context ctx, Entity e, MoveAction& move)
{
  auto pose = ctx.entities.get<Pose>(e);
  if (!pose) return ActionResult::Canceled;
  pose->position += glm::normalize(move.direction);
  return ActionResult::Complete;
}

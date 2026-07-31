#include "action/move.hh"
#include "component/pose.hh"
#include "context.hh"
#include <glm/geometric.hpp>

ActionResult act(Context ctx, Handle<Entity> e, MoveAction& move)
{
  auto pose = ctx.entities.try_get<Pose>(e);
  if (!pose) return ActionResult::Canceled;
  pose->position += glm::normalize(move.direction);
  return ActionResult::Complete;
}

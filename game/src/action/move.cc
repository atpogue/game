#include "action/move.hh"
#include "component/pose.hh"
#include "context.hh"
#include "entity.hh"
#include <glm/geometric.hpp>

ActionResult act(Context ctx, Handle<Entity> e, MoveAction& move)
{
  auto view = access_entity(ctx, e);
  auto pose = view.try_get<Pose>();
  if (!pose) return ActionResult::Canceled;
  pose->position += glm::normalize(move.direction);
  return ActionResult::Complete;
}

#include "action/move.hh"
#include "component/pose.hh"
#include <glm/geometric.hpp>

ActionResult act(GameRegistry &r, Entity e, MoveAction &move) {
    auto pose = r.get<Pose>(e);
    if (!pose) return ActionResult::Canceled;
    pose->position += glm::normalize(move.direction);
    return ActionResult::Complete;
}


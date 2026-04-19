#include "control/action.hh"
#include "ecs/pose.hh"
#include <glm/geometric.hpp>

MoveAction::MoveAction(glm::vec2 to) : direction_(glm::normalize(to)) {}

MoveAction::MoveAction(f32 x, f32 y) : MoveAction(glm::vec2(x,y)) {}

Action::Status MoveAction::status() const { return Action::Status::Busy; }

Action::Status MoveAction::cancel() { return Action::Status::Success; }

void MoveAction::step(Entity e) {
    auto pose = poses::get(e);
    if (!pose) return;
    pose->position += glm::vec2(direction_.x, direction_.y);
}


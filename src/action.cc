#include "action.hh"
#include "transform.hh"
#include <glm/geometric.hpp>

bool MoveAction::is_compatible(Entity e) { return ledger::has(e, Component::Transform); }

MoveAction::MoveAction(glm::vec2 to) : direction_(glm::normalize(to)) {}

MoveAction::MoveAction(float x, float y) : MoveAction(glm::vec2(x,y)) {}

Action::Status MoveAction::status() const { return Action::Status::Busy; }

Action::Status MoveAction::cancel() { return Action::Status::Success; }

void MoveAction::step(Entity e) {
    auto transform = transforms::get(e);
    if (!transform) return;
    transform->position += 5.f * glm::vec2(direction_.x, direction_.y);
}


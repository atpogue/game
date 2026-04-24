#include "control/move.hh"
#include "ecs/pose.hh"
#include <glm/geometric.hpp>

Action *make_action(Entity e, const Command::Move &move) {
    if (ledger::has(e, Flag::Pose))
        return new MoveAction(move.x, move.y);
    return nullptr;
}

MoveAction::MoveAction(glm::vec2 to) : direction(glm::normalize(to)), status{false} {}

MoveAction::MoveAction(f32 x, f32 y) : MoveAction(glm::vec2(x,y)) {}

bool MoveAction::is_complete() const { return status; }

bool MoveAction::cancel() { return true; }

void MoveAction::step(Entity e) {
    if (status) return; status = true;
    auto pose = poses::get(e); if (!pose) return;
    pose->position += direction;
}


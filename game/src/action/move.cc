#include "command.hh"
#include "move.hh"
#include "engine/component/pose.hh"
#include <glm/geometric.hpp>

std::unique_ptr<Action> make_action(const Registry &r, Entity e, const Command::Move &move) {
    if (r.has<Pose>(e))
        return std::make_unique<MoveAction>(r, e, glm::vec2(move.x, move.y), 4u);
    return nullptr;
}

namespace {

    glm::vec2 get_destination(const Registry &r, Entity e, glm::vec2 direction) {
        auto pose = r.get<Pose>(e);
        [[unlikely]] if (!pose) {
            assert(!"move action on incompatible entity");
            return {0.f, 0.f};
        }
        return glm::normalize(direction) + pose->position;
    }

}

MoveAction::MoveAction(const Registry &r, Entity e, glm::vec2 direction, u8 duration)
    : duration_{duration}
    , to_(get_destination(r, e, direction))
    , speed_(glm::normalize(direction) / (f32)duration_)
{
}

void MoveAction::step(Registry &r, Entity e) {
    if (is_complete()) return;
    auto pose = r.get<Pose>(e);
    [[unlikely]] if (!pose) {
        assert(!"move action on incompatible entity");
        duration_ = 0u;
        return;
    }

    if (duration_ > 1u) pose->position += speed_;
    else pose->position = to_;
    --duration_;
}


#include "engine/component/actor.hh"
#include <cassert>

Actor::Actor(u8 size) {
    buffer_.reserve(size);
}

bool Actor::try_submit(std::unique_ptr<Action> action) {
    if (!action) return false;
    if (buffer_.size() == buffer_.capacity()) return false;
    buffer_.emplace_back(action.release());
    return true;
}

void Actor::step(Registry &r, Entity e) {
    for (auto &action : buffer_) {
        assert(action && "actor's buffer has null action");
        action->step(r, e);
    }
    std::erase_if(buffer_, [](const auto &action) { return action->is_complete(); });
}


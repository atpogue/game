#pragma once
#include "entity.hh"
#include <glm/vec2.hpp>

struct MoveComponent {

    glm::vec2 direction;
    bool active = false;

};

namespace movement {

    MoveComponent *get(Entity e);
    void set(Entity e, MoveComponent &&c);

} // namespace movement


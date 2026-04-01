#pragma once
#include "ledger.hh"
#include <glm/vec2.hpp>

struct MoveComponent {

    glm::vec2 position;
    // glm::vec2 rotation;

};

namespace transforms {

    MoveComponent *get(Entity e);
    MoveComponent *set(Entity e, MoveComponent &&c);
    void destroy(Entity e);
    void step();

} // namespace transforms


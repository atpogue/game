#pragma once
#include "engine/ecs/ledger.hh"
#include <glm/vec2.hpp>

struct Pose {

    glm::vec2 position;
    // glm::vec2 rotation;

};

namespace poses {

    Pose *get(Entity e);
    Pose *set(Entity e, Pose &&c);
    void destroy(Entity e);
    void step();

} // namespace poses


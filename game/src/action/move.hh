#pragma once
#include "action/result.hh"
#include <glm/vec2.hpp>

struct MoveAction {

    glm::vec2 direction = {0.f, 0.f};
    float speed = 1.f;

};

struct Context;
struct Entity;

ActionResult act(Context ctx, Entity e, MoveAction &move);


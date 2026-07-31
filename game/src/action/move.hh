#pragma once
#include "action/result.hh"
#include "types.hh"
#include <glm/vec2.hpp>

struct MoveAction
{
  glm::vec2 direction = {0.f, 0.f};
  float     speed     = 1.f;
};

ActionResult act(Context ctx, Handle<Entity> e, MoveAction& move);

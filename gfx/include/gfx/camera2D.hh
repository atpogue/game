#pragma once
#include "core/types.hh"
#include <glm/vec2.hpp>

struct Camera2D
{
  glm::vec2 position; // the world position the camera is centered on
  f32       zoom;
};


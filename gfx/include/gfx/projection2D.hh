#pragma once
#include "gfx/camera2D.hh"
#include "gfx/rectangle.hh"
#include <glm/vec2.hpp>

struct Projection2D
{
  glm::vec2 extent;          // dimensions in pixels
  f32       pixels_per_unit; // needed to translate pixel coordinates to world coordinates

  [[nodiscard]] glm::vec2 to_world_space(Camera2D const& camera, glm::vec2 pixel) const noexcept
  {
    return view_to_world(camera, screen_to_view(pixel));
  }

  [[nodiscard]] glm::vec2 to_screen_space(Camera2D const& camera, glm::vec2 unit) const noexcept
  {
    return view_to_screen(world_to_view(camera, unit));
  }

  [[nodiscard]] Rectangle clip(Camera2D const& camera) const noexcept
  {
    glm::vec2 const lo = to_world_space(camera, { 0.f, 0.f });
    glm::vec2 const hi = to_world_space(camera, extent);
    return { lo, hi - lo };
  }

private:

  [[nodiscard]] glm::vec2 view_to_screen(glm::vec2 unit) const noexcept
  {
    return unit * pixels_per_unit + extent * 0.5f;
  }

  [[nodiscard]] glm::vec2 screen_to_view(glm::vec2 pixel) const noexcept
  {
    return (pixel - extent * 0.5f) / pixels_per_unit;
  }

  [[nodiscard]] glm::vec2 world_to_view(Camera2D const& camera, glm::vec2 unit) const noexcept
  {
    return (unit - camera.position) * camera.zoom;
  }

  [[nodiscard]] glm::vec2 view_to_world(Camera2D const& camera, glm::vec2 unit) const noexcept
  {
    return unit / camera.zoom + camera.position;
  }
};


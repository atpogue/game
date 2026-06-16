#include "engine/render/camera.hh"
#include "core/math.hh"
#include "core/panic.hh"
#include "internal.hh"

// Places a world tile at 1:1 in the off-screen scene target (zoom is applied
// later, once, by `scene_present`), centred on the oversized target.
Vec2f Camera::view_coord_at(Vec2f world_coord, float tile_size) const
{
  return (world_coord - position) * tile_size + detail::scene_size() * 0.5f;
}

// Inverse of `view_coord_at`. A mouse-pick path must first map the window pixel
// to a scene pixel: `scene_xy = src.xy + mouse_px / zoom`.
Vec2f Camera::world_coord_at(Vec2f scene_pixel, float tile_size) const
{
  return (scene_pixel - detail::scene_size() * 0.5f) / tile_size + position;
}

// The visible half-extent in tiles: `viewport` is the view at zoom 1, so the
// actual visible area shrinks as `zoom` grows. This drives culling.
bool Camera::contains(Vec2f world_coord) const
{
  Vec2f half = (viewport * 0.5f) / zoom;
  Vec2f rel  = world_coord - position;
  return rel.x >= -half.x && rel.x <= half.x && rel.y >= -half.y && rel.y <= half.y;
}

Camera::Iterator Camera::begin() const
{
  PRECONDITION(zoom > 0.f);
  PRECONDITION(viewport.x > 0.f && viewport.y > 0.f);
  Vec2f half = (viewport * 0.5f) / zoom;
  Vec2f min  = glm::floor(position - half);
  Vec2f max  = glm::ceil(position + half);
  return Iterator(min, max, min);
}

Camera::Iterator Camera::end() const
{
  PRECONDITION(zoom > 0.f);
  PRECONDITION(viewport.x > 0.f && viewport.y > 0.f);
  Vec2f half = (viewport * 0.5f) / zoom;
  Vec2f min  = glm::floor(position - half);
  Vec2f max  = glm::ceil(position + half);
  return Iterator(min, max, Vec2f{min.x, max.y + 1.0f});
}

Camera::Iterator::Iterator(Vec2f min, Vec2f max, Vec2f start)
  : min_x{min.x}, max_x{max.x}, position{start}
{
}

Vec2f Camera::Iterator::operator*() const { return position; }

Camera::Iterator& Camera::Iterator::operator++()
{
  if (++position.x > max_x) {
    ++position.y;
    position.x = min_x;
  }
  return *this;
}

Camera::Iterator Camera::Iterator::operator++(int)
{
  auto temp = *this;
  ++(*this);
  return temp;
}

bool Camera::Iterator::operator==(Iterator const& other) const
{
  return position == other.position;
}

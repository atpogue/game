#include "engine/render/camera.hh"

#include "engine/core/error.hh"
#include "engine/core/math.hh"

Vec2f Camera::view_coord_at(Vec2f world_coord) const
{
  return (world_coord - position) * zoom + viewport * 0.5f;
}

Vec2f Camera::world_coord_at(Vec2f view_coord) const
{
  return (view_coord - viewport * 0.5f) / zoom + position;
}

bool Camera::contains(Vec2f world_coord) const
{
  Vec2f half       = viewport * 0.5f;
  auto  view_coord = (world_coord - position) * zoom + half;
  return view_coord.x >= 0.f && view_coord.x <= viewport.x && view_coord.y >= 0.f
      && view_coord.y <= viewport.y;
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
  : min_x{min.x}
  , max_x{max.x}
  , position{start}
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

bool Camera::Iterator::operator==(const Iterator& other) const
{
  return position == other.position;
}


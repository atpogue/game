#pragma once
#include <glm/vec2.hpp>
#include <iterator>

struct Camera
{
  // `viewport`: the visible area in tiles at zoom 1 — the cull region.
  //   The tile iterator yields tiles within `viewport / zoom` of `position`.
  glm::vec2 position, viewport;
  float     zoom = 1.0f;
  // Maps a world tile coordinate to its pixel position in the off-screen scene
  // target (the world is centred in it), and the inverse.
  glm::vec2 view_coord_at(glm::vec2 world_coord, float tile_size) const;
  glm::vec2 world_coord_at(glm::vec2 scene_pixel, float tile_size) const;
  bool      contains(glm::vec2 world_coord) const;

  struct Iterator
  {
    using iterator_category                    = std::forward_iterator_tag;
    using value_type                           = glm::vec2;
    Iterator(Iterator const& other)            = default;
    Iterator& operator=(Iterator const& other) = default;
    glm::vec2 operator*() const;
    Iterator& operator++();
    Iterator  operator++(int);
    bool      operator==(Iterator const& other) const;

  private:

    friend struct Camera;
    Iterator(glm::vec2 min, glm::vec2 max, glm::vec2 start);
    float     min_x, max_x;
    glm::vec2 position;
  };

  Iterator begin() const;
  Iterator end() const;
};

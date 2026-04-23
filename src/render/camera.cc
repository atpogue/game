#include "camera.hh"
#include <glm/common.hpp>

glm::vec2 Camera::view_coord_at(glm::vec2 world_coord) const {
    return (world_coord - position) * zoom + viewport * 0.5f;
}

glm::vec2 Camera::world_coord_at(glm::vec2 view_coord) const {
    return (view_coord - viewport * 0.5f) / zoom + position;
}

bool Camera::contains(glm::vec2 world_coord) const {
    glm::vec2 half = viewport * 0.5f;
    auto view_coord = (world_coord - position) * zoom + half;
    return view_coord.x >= 0.f && view_coord.x <= viewport.x
        && view_coord.y >= 0.f && view_coord.y <= viewport.y;
}

Camera::Iterator Camera::begin() const {
    assert(zoom > 0.f);
    assert(viewport.x > 0.f && viewport.y > 0.f);
    glm::vec2 half = (viewport * 0.5f) / zoom;
    glm::vec2 min = glm::floor(position - half);
    glm::vec2 max = glm::ceil(position + half);
    return Iterator(min, max, min);
}

Camera::Iterator Camera::end() const {
    assert(zoom > 0.f);
    assert(viewport.x > 0.f && viewport.y > 0.f);
    glm::vec2 half = (viewport * 0.5f) / zoom;
    glm::vec2 min = glm::floor(position - half);
    glm::vec2 max = glm::ceil(position + half);
    return Iterator(min, max, glm::vec2{min.x, max.y + 1.0f});
}

Camera::Iterator::Iterator(glm::vec2 min, glm::vec2 max, glm::vec2 start)
    : min_x{min.x}
    , max_x{max.x}
    , position{start}
{}

glm::vec2 Camera::Iterator::operator*() const { return position; }

Camera::Iterator &Camera::Iterator::operator++() {
    if (++position.x > max_x) {
        ++position.y;
        position.x = min_x;
    }
    return *this;
}

Camera::Iterator Camera::Iterator::operator++(int) { auto temp = *this; ++(*this); return temp; }

bool Camera::Iterator::operator==(const Iterator &other) const {
    return position == other.position;
}


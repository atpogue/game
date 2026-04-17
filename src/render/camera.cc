#include "camera.hh"
#include <cmath>

glm::vec2 Camera::view_coord_at(glm::vec2 world_coord) const {
    return (world_coord - position) * zoom + viewport * 0.5f;
}

glm::vec2 Camera::world_coord_at(glm::vec2 view_coord) const {
    return (view_coord - viewport * 0.5f) / zoom + position;
}

bool Camera::contains(glm::vec2 world_coord) const {
    glm::vec2 half = viewport * 0.5f;
    auto view_coord = (world_coord - position) * zoom + half;
    return view_coord.x <= ceilf(position.x + half.x)
        && view_coord.x >= floorf(position.x - half.x)
        && view_coord.y <= ceilf(position.y + half.y)
        && view_coord.y >= floorf(position.y - half.y);
}

Camera::Iterator Camera::begin() const {
    assert(zoom > 0.f);
    assert(viewport.x > 0.f && viewport.y > 0.f);
    glm::vec2 half = (viewport * 0.5f) / zoom;
    return Iterator(position - half, position + half);
}

Camera::Iterator Camera::end() const {
    assert(zoom > 0.f);
    assert(viewport.x > 0.f && viewport.y > 0.f);
    glm::vec2 half = (viewport * 0.5f) / zoom;
    return Iterator(position - half, position + half + 1.0f);
}

Camera::Iterator::Iterator(glm::vec2 from, glm::vec2 to)
    : min_x{fminf(from.x, to.x)}
    , max_x{fmaxf(from.x, to.x)}
    , position{min_x, fminf(from.y, to.y)}
{}

glm::vec2 Camera::Iterator::operator*() { return position; }

Camera::Iterator &Camera::Iterator::operator++() {
    if (position.x < max_x) ++position.x;
    else {
        ++position.y;
        position.x = min_x;
    }
    return *this;
}

Camera::Iterator Camera::Iterator::operator++(int) { auto temp = *this; ++(*this); return temp; }

bool Camera::Iterator::operator==(const Iterator &other) const {
    return floorf(position.x) == floorf(other.position.x)
        && floorf(position.y) == floorf(other.position.y);
}


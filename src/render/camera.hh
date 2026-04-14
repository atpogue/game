#pragma once
#include <glm/vec2.hpp>
#include <iterator>

struct Camera {

    glm::vec2 position, viewport;
    float zoom = 1.0f;

    glm::vec2 view_coord_at(glm::vec2 world_coord) const;
    glm::vec2 world_coord_at(glm::vec2 view_coord) const;

    bool contains(glm::vec2 world_coord) const;

    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = glm::vec2;

        Iterator(glm::vec2 from, glm::vec2 to);
        Iterator(const Iterator &other) = default;
        Iterator &operator=(const Iterator &other) = default;

        glm::vec2 operator*();

        Iterator &operator++();
        Iterator operator++(int);

        bool operator==(const Iterator &other) const;

    private:

        float min_x, max_x;
        glm::vec2 position;
    };

    Iterator begin() const;
    Iterator end() const;

};


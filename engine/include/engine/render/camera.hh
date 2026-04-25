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

        Iterator(const Iterator &other) = default;
        Iterator &operator=(const Iterator &other) = default;

        glm::vec2 operator*() const;

        Iterator &operator++();
        Iterator operator++(int);

        bool operator==(const Iterator &other) const;

    private:

        friend struct Camera;

        Iterator(glm::vec2 min, glm::vec2 max, glm::vec2 start);

        float min_x, max_x;
        glm::vec2 position;

    };

    Iterator begin() const;
    Iterator end() const;

};


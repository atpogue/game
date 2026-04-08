#pragma once
#include <cassert>
#include <gml/vec2.hpp>

template <typename T>
struct Grid2 {
    // could just use an std::mdspan instead

    Grid2(unsigned w, unsigned h)
        : width_{w}, height_{h}
        , data{new T[size()]()}
    {}

    Grid2(unsigned w, unsigned h, const T &value)
        : width_{w}, height_{h}
        , data{new T[size()](value)}
    {}

    Grid2(const Grid2 &other)
        : width_{other.width_}, height_{other.height_}
        , data{new T[size()]()}
    {
        for (size_t i = 0; i < size(); i++) data[i] = other.data[i];
    }

    Grid2 &operator=(const Grid2 &other) {
        if (this == &other) return;
        delete[] data;
        width_ = other.width_;
        height_ = other.height_;
        data = new T[size()]();
        for (size_t i = 0; i < size(); i++) data[i] = other.data[i];
        return *this;
    }

    Grid2(Grid2 &&other) noexcept
        : width_{other.width_}, height_{other.height_}
        , data{other.data}
    {
        other.width_ = 0u;
        other.height_ = 0u;
        other.data = nullptr;
    }

    Grid2 &operator=(Grid2 &&other) noexcept {
        if (this == &other) return;
        delete[] data;
        width_ = other.width_;
        height_ = other.height_;
        data = other.data;
        other.width_ = 0u;
        other.height_ = 0u;
        other.data = nullptr;
        return *this;
    }

    ~Grid2() { delete[] data; }

    constexpr size_t size() const {
        assert(data != nullptr || size() == 0);
        return size_t(width_) * size_t(height_);
    }

    constexpr bool has(int x, int y) const {
        assert(data != nullptr || size() == 0);
        return x >= 0 && x < width_
            && y >= 0 && y < height_;
    }

    constexpr bool has(glm::uvec2 coord) const { return has(coord.x, coord.y); }

    constexpr const T &operator[](int x, int y) const { assert(has(x, y)); return data[index(x, y)]; }
    constexpr       T &operator[](int x, int y)       { assert(has(x, y)); return data[index(x, y)]; }

    constexpr const T &operator[](glm::uvec2 coord) const { return (*this)[coord.x, coord.y]; }
    constexpr       T &operator[](glm::uvec2 coord)       { return (*this)[coord.x, coord.y]; }

    constexpr const T *get(int x, int y) const { return has(x,y) ? data + index(x,y) : nullptr; }
    constexpr       T *get(int x, int y)       { return has(x,y) ? data + index(x,y) : nullptr; }

    constexpr const T *get(glm::uvec2 coord) const { return get(coord.x, coord.y); }
    constexpr       T *get(glm::uvec2 coord)       { return get(coord.x, coord.y); }

    constexpr const T *begin() const { return data; }
    constexpr       T *begin()       { return data; }

    constexpr const T *end() const { return data + size(); }
    constexpr       T *end()       { return data + size(); }

    constexpr unsigned width() const { return width_; }
    constexpr unsigned height() const { return height_; }

private:

    unsigned width_, height_;
    T *data;

    constexpr size_t index(int x, int y) const {
        size_t i = x + (y * width_);
        assert(i < size() && "invalid index");
        return i;
    }

};


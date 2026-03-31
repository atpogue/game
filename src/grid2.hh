#pragma once
#include <cassert>
#include <gml/vec2.hpp>

template <typename T>
struct Grid2 {

    Grid2(unsigned w, unsigned h)
        : width_{w}, height_{h}
        , data_{new T[size()]()}
    {}

    Grid2(unsigned w, unsigned h, const T &value)
        : width_{w}, height_{h}
        , data_{new T[size()](value)}
    {}

    Grid2(const Grid2 &other)
        : width_{other.width_}, height_{other.height_}
        , data_{new T[size()]()}
    {
        for (size_t i = 0; i < size(); i++) data_[i] = other.data_[i];
    }

    Grid2(Grid2 &&other)
        : width_{other.width_}, height_{other.height_}
        , data_{other.data_}
    {
        assert(data_ != nullptr);
        other.data_ = nullptr;
    }

    ~Grid2() { delete[] data_; }

    void operator=(Grid2 &&other) {
        width_ = other.width_;
        height_ = other.height_;
        delete[] data_;
        data_ = other.data_;
        other.data_ = nullptr;
    }

    void operator=(const Grid2 &other) {
        width_ = other.width_;
        height_ = other.height_;
        delete[] data_;
        data_ = new T[size()]();
        for (size_t i = 0; i < size(); i++) data_[i] = other.data_[i];
    }

    size_t size() const { return size_t(width_) * size_t(height_); }

    bool has(int x, int y) const {
        return x >= 0 && x < width_
            && y >= 0 && y < height_;
    }

    bool has(glm::uvec2 coord) const { return has(coord.x, coord.y); }

    const T &operator[](glm::uvec2 coord) const { assert(has(coord)); return data_[index(coord)]; }
          T &operator[](glm::uvec2 coord)       { assert(has(coord)); return data_[index(coord)]; }

    const T *get(int x, int y) const { return has(x,y) ? data_ + index(x,y) : nullptr; }
          T *get(int x, int y)       { return has(x,y) ? data_ + index(x,y) : nullptr; }

    const T *get(glm::uvec2 coord) const { return get(coord.x, coord.y); }
          T *get(glm::uvec2 coord)       { return get(coord.x, coord.y); }

    const T *begin() const { return data_; }
          T *begin()       { return data_; }

    const T *end() const { return data_ + size(); }
          T *end()       { return data_ + size(); }

    unsigned width() const { return width_; }
    unsigned height() const { return height_; }

private:

    unsigned width_, height_;
    T *data_;

    size_t index(int x, int y) const {
        size_t i = x + (y * width_);
        assert(i < size());
        return i;
    }

    size_t index(glm::uvec2 coord) const { return index(coord.x, coord.y); }

};


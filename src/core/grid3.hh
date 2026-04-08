#pragma once
#include <cassert>
#include <glm/vec3.hpp>

template <typename T>
struct Grid3 {

    Grid3(unsigned w, unsigned h, unsigned d)
        : width_{w}, height_{h}, depth_{d}
        , data_{new T[size()]()}
    {}

    Grid3(unsigned w, unsigned h, unsigned d, const T &value)
        : width_{w}, height_{h}, depth_{d}
        , data_{new T[size()](value)}
    {}

    Grid3(const Grid3 &other)
        : width_{other.width_}, height_{other.height_}, depth_{other.depth_}
        , data_{new T[size()]()}
    {
        for (size_t i = 0; i < size(); i++) data_[i] = other.data_[i];
    }

    Grid3 &operator=(const Grid3 &other) {
        if (this == &other) return;
        delete[] data_;
        width_ = other.width_;
        height_ = other.height_;
        depth_ = other.depth_;
        data_ = new T[size()]();
        for (size_t i = 0; i < size(); i++) data_[i] = other.data_[i];
        return *this;
    }

    Grid3(Grid3 &&other) noexcept
        : width_{other.width_}, height_{other.height_}, depth_{other.depth_}
        , data_{other.data_}
    {
        other.width_ = 0u;
        other.height_ = 0u;
        other.depth_ = 0u;
        other.data_ = nullptr;
    }

    Grid3 &operator=(Grid3 &&other) noexcept {
        if (this == &other) return;
        delete[] data_;
        width_ = other.width_;
        height_ = other.height_;
        depth_ = other.depth_;
        data_ = other.data_;
        other.width_ = 0u;
        other.height_ = 0u;
        other.depth_ = 0u;
        other.data_ = nullptr;
        return *this;
    }

    ~Grid3() noexcept { delete[] data_; }

    constexpr size_t size() const {
        assert(data_ != nullptr || size() == 0); // this should never happen!
        return size_t(width_) * size_t(height_) * size_t(depth_);
    }

    constexpr bool has(int x, int y, int z) const {
        assert(data_ != nullptr || size() == 0);
        return x >= 0 && x < width_
            && y >= 0 && y < height_
            && z >= 0 && z < depth_;
    }

    constexpr bool has(glm::uvec3 coord) const { return has(coord.x, coord.y, coord.z); }

    constexpr const T &operator[](int x, int y, int z) const { assert(has(x, y, z)); return data_[index(x, y, z)]; }
    constexpr       T &operator[](int x, int y, int z)       { assert(has(x, y, z)); return data_[index(x, y, z)]; }

    constexpr const T &operator[](glm::uvec3 coord) const { return (*this)[coord.x, coord.y, coord.z]; }
    constexpr       T &operator[](glm::uvec3 coord)       { return (*this)[coord.x, coord.y, coord.z]; }

    constexpr const T *get(int x, int y, int z) const { return has(x,y,z) ? data_ + index(x,y,z) : nullptr; }
    constexpr       T *get(int x, int y, int z)       { return has(x,y,z) ? data_ + index(x,y,z) : nullptr; }

    constexpr const T *get(glm::uvec3 coord) const { return get(coord.x, coord.y, coord.z); }
    constexpr       T *get(glm::uvec3 coord)       { return get(coord.x, coord.y, coord.z); }

    constexpr const T *begin() const { return data_; }
    constexpr       T *begin()       { return data_; }

    constexpr const T *end() const { return data_ + size(); }
    constexpr       T *end()       { return data_ + size(); }

    constexpr unsigned width() const { return width_; }
    constexpr unsigned height() const { return height_; }
    constexpr unsigned depth() const { return depth_; }

private:

    unsigned width_, height_, depth_;
    T *data_;

    constexpr size_t index(int x, int y, int z) const {
        size_t i = x + (y * width_) + (z * width_ * height_);
        assert(i < size() && "invalid index");
        return i;
    }

};


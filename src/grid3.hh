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
        : width{other.width_}, height{other.height_}, depth{other.depth_}
        , data_{new T[size()]()}
    {
        for (size_t i = 0; i < size(); i++) data_[i] = other.data_[i];
    }

    Grid3(Grid3 &&other) 
        : width{other.width_}, height{other.height_}, depth{other.depth_}
        , data_{other.data_}
    {
        assert(data_ != nullptr);
        other.data_ = nullptr;
    }

    ~Grid3() { delete[] data_; }

    void operator=(Grid3 &&other) {
        width = other.width_;
        height = other.height_;
        depth = other.depth_;
        delete[] data_;
        data_ = other.data_;
        other.data_ = nullptr;
    }

    void operator=(const Grid3 &other) {
        width = other.width_;
        height = other.height_;
        depth = other.depth_;
        delete[] data_;
        data_ = new T[size()]();
        for (size_t i = 0; i < size(); i++) data_[i] = other.data_[i];
    }

    size_t size() const { return size_t(width) * size_t(height) * size_t(depth); }

    bool has(int x, int y, int z) const {
        return x >= 0 && x < width_
            && y >= 0 && y < height_
            && z >= 0 && z < depth_;
    }

    bool has(glm::uvec3 coord) const { return has(coord.x, coord.y, coord.z); }

    const T &operator[](glm::uvec3 coord) const { assert(has(coord)); return data_[index(coord)]; }
          T &operator[](glm::uvec3 coord)       { assert(has(coord)); return data_[index(coord)]; }

    const T *get(int x, int y, int z) const { return has(x,y,z) ? data_ + index(x,y,z) : nullptr; }
          T *get(int x, int y, int z)       { return has(x,y,z) ? data_ + index(x,y,z) : nullptr; }

    const T *get(glm::uvec3 coord) const { return get(coord.x, coord.y, coord.z); }
          T *get(glm::uvec3 coord)       { return get(coord.x, coord.y, coord.z); }

    const T *begin() const { return data_; }
          T *begin()       { return data_; }

    const T *end() const { return data_ + size(); }
          T *end()       { return data_ + size(); }

    unsigned width() const { return width_; }
    unsigned height() const { return height_; }
    unsigned depth() const { return depth_; }

private:

    unsigned width_, height_, depth_;
    T *data_;

    size_t index(int x, int y, int z) const {
        size_t i = x + (y * width) + (z * width * height);
        assert(i < size());
        return i;
    }

    size_t index(glm::uvec3 coord) const { return index(coord.x, coord.y, coord.z); }

};


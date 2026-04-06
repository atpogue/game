#pragma once
#include <concepts>
#include <cstdint>
#include <limits>
#include <vector>

template <typename Type>
struct SparseMap {

    struct Item { uint32_t key; Type value; };

    constexpr size_t capacity() const { return dense.capacity(); }

    void reserve(uint32_t num_values, uint32_t num_keys = 0) {
        dense.reserve(num_values);
        sparse.reserve(num_keys);
    }

    bool has(uint32_t key) const {
        assert(key >= sparse.size() || sparse[key] == index_max || sparse[key] < dense.size());
        return key < sparse.size() && sparse[key] < index_max;
    }

    // assigns the value to the key if it does, creates the key if it doesn't yet exist
    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Type &emplace(uint32_t key, Args &&... args) {
        if (key >= sparse.size())
            sparse.resize(key + 1, index_max);

        uint32_t i = sparse[key];
        if (i >= index_max) {
            i = dense.size();
            sparse[key] = i;
            dense.emplace_back(key, Type(std::forward<Args>(args)...));
        } else {
            dense[i].value = Type(std::forward<Args>(args)...);
        }

        assert(i < index_max && i < dense.size());
        return dense[i].value;
    }

    void erase(uint32_t key) {
        if (key >= sparse.size()) return;
        uint32_t &i = sparse[key];
        if (i == index_max) return;
        assert(i < dense.size());
        sparse[key] = index_max;
        sparse[dense.back().key] = i;
        dense[i].key = dense.back().key;
        dense[i].value = std::move(dense.back().value);
        dense.pop_back();
    }

    const Type *get(uint32_t key) const { return has(key) ? &dense[sparse[key]].value : nullptr; }
          Type *get(uint32_t key)       { return has(key) ? &dense[sparse[key]].value : nullptr; }

    constexpr size_t size() const { return dense.size(); }

    constexpr auto begin() const { return dense.begin(); }
    constexpr auto begin()       { return dense.begin(); }

    constexpr auto end() const { return dense.end(); }
    constexpr auto end()       { return dense.end(); }

private:

    static constexpr uint32_t index_max = std::numeric_limits<uint32_t>::max();

    std::vector<Item> dense;
    // if the largest index used is huge the sparse array will use a lot of memory
    std::vector<uint32_t> sparse;

};


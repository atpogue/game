#pragma once
#include "engine/core/types.hh"
#include <cassert>
#include <concepts>
#include <vector>

template <typename Type>
struct SparseMap {

    struct Item { u32 key; Type value; };

    constexpr size_t capacity() const { return dense.capacity(); }

    void reserve(u32 num_values, u32 num_keys = 0) {
        dense.reserve(num_values);
        sparse.reserve(num_keys);
    }

    bool has(u32 key) const {
        assert(key >= sparse.size() || sparse[key] == index_max || sparse[key] < dense.size());
        return key < sparse.size() && sparse[key] < index_max;
    }

    // assigns the value to the key if it does, creates the key if it doesn't yet exist
    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Type &emplace(u32 key, Args &&... args) {
        if (key >= sparse.size())
            sparse.resize(key + 1, index_max);

        u32 i = sparse[key];
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

    void erase(u32 key) {
        if (key >= sparse.size()) return;
        u32 &i = sparse[key];
        if (i == index_max) return;
        assert(i < dense.size());
        sparse[key] = index_max;
        sparse[dense.back().key] = i;
        dense[i].key = dense.back().key;
        dense[i].value = std::move(dense.back().value);
        dense.pop_back();
    }

    void clear() { dense.clear(); sparse.clear(); }

    const Type *get(u32 key) const { return has(key) ? &dense[sparse[key]].value : nullptr; }
          Type *get(u32 key)       { return has(key) ? &dense[sparse[key]].value : nullptr; }

    constexpr size_t size() const { return dense.size(); }

    constexpr auto begin() const { return dense.begin(); }
    constexpr auto begin()       { return dense.begin(); }

    constexpr auto end() const { return dense.end(); }
    constexpr auto end()       { return dense.end(); }

private:

    static constexpr u32 index_max = UINT32_MAX;

    std::vector<Item> dense;
    // if the largest index used is huge the sparse array will use a lot of memory
    std::vector<u32> sparse;

};


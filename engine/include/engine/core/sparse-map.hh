#pragma once
#include "engine/core/types.hh"
#include <cassert>
#include <utility>
#include <concepts>
#include <vector>

template <typename Type>
struct SparseMap {

    struct Item { u32 key; Type value; };

    [[nodiscard]] constexpr size_t capacity() const { return dense.capacity(); }

    void reserve(u32 num_values, u32 num_keys = 0) {
        dense.reserve(num_values);
        sparse.reserve(num_keys);
    }

    [[nodiscard]] constexpr bool has(u32 key) const { return key < sparse.size() && sparse[key] != nil; }

    // assigns the value to the key if it does, creates the key if it doesn't yet exist
    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Type &emplace(u32 key, Args &&... args) {
        assert(key != nil && "emplace at nil index");

        if (key >= sparse.size()) {
            // grow sparse array to include the key
            sparse.resize(sparse_page_size * (1u + key / sparse_page_size), nil);
        }

        u32 i = sparse[key];
        if (i == nil) {
            i = dense.size();
            sparse[key] = i;
            dense.emplace_back(key, Type(std::forward<Args>(args)...));
        } else {
            dense[i].value = Type(std::forward<Args>(args)...);
        }

        assert(i < dense.size());
        return dense[i].value;
    }

    void erase(u32 key) {
        if (key >= sparse.size()) return;
        u32 &i = sparse[key];
        if (i == nil) return;
        assert(i < dense.size());
        sparse[key] = nil;
        sparse[dense.back().key] = i;
        dense[i].key = dense.back().key;
        dense[i].value = std::move(dense.back().value);
        dense.pop_back();
    }

    void clear() { dense.clear(); sparse.clear(); }

    [[nodiscard]] const Type &operator[](u32 key) const { assert(has(key)); return dense[sparse[key]].value; }
    [[nodiscard]]       Type &operator[](u32 key)       { assert(has(key)); return dense[sparse[key]].value; }

    [[nodiscard]] const Type *get(u32 key) const { return has(key) ? &dense[sparse[key]].value : nullptr; }
    [[nodiscard]]       Type *get(u32 key)       { return has(key) ? &dense[sparse[key]].value : nullptr; }

    [[nodiscard]] constexpr size_t size() const { return dense.size(); }

    using iterator = std::vector<Item>::iterator;
    using const_iterator = std::vector<Item>::const_iterator;

    [[nodiscard]] constexpr const_iterator begin() const { return dense.begin(); }
    [[nodiscard]] constexpr iterator       begin()       { return dense.begin(); }

    [[nodiscard]] constexpr const_iterator end() const { return dense.end(); }
    [[nodiscard]] constexpr iterator       end()       { return dense.end(); }

private:

    static constexpr u32 sparse_page_size = 256; // 256 * 4B = 1 KB of memory

    std::vector<Item> dense;
    // if the largest index used is huge the sparse array will use a lot of memory
    std::vector<u32> sparse;

};


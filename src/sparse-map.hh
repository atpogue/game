#pragma once
#include <concepts>
#include <limits>

// ignores the key value of 0
template <typename Value, std::unsigned_integral Key=size_t>
struct SparseMap {

    struct Entry { Key key; Value value; };

    void reserve(Key num_values, Key num_keys = 0) {
        data_.reserve(num_values);
        index_.reserve(num_keys);
    }

    bool has(Key key) const {
        assert(key >= index_.size() || index_[key] == NULL_INDEX || index_[key] < data_.size());
        return key < index_.size() && index_[key] != NULL_INDEX;
    }

    // creates the key if it doesn't yet exist
    Value &operator[](Key key) {
        if (key >= index_.size()) index_.resize(key + 1, NULL_INDEX);

        Key &i = index_[key];
        if (i == NULL_INDEX) {
            i = data_.size();
            data_.emplace_back(key);
        }

        assert(i != NULL_INDEX && i < data_.size());
        return data_[i].value;
    }

    // does what operator[] without creating a default constructed value
    Value &set(Key key, Value &&value) {
        if (key >= index_.size()) index_.resize(key + 1, NULL_INDEX);

        Key &i = index_[key];
        if (i == NULL_INDEX) {
            i = data_.size();
            data_.emplace_back(key, std::move(value));
        } else {
            data_[i].value = std::move(value);
        }

        assert(i != NULL_INDEX && i < data_.size());
        return data_[i].value;
    }

    void erase(Key key) {
        if (key >= index_.size()) return;
        Key &i = index_[key];
        if (i == NULL_INDEX) return;
        assert(i < data_.size());
        index_[key] = NULL_INDEX;
        index_[data_.back().key] = i;
        data_[i].key = data_.back().key;
        data_[i].value = std::move(data_.back().value);
        data_.pop_back();
    }

    const Value *get(Key key) const { return has(key) ? &data_[index_[key]].value : nullptr; }
          Value *get(Key key)       { return has(key) ? &data_[index_[key]].value : nullptr; }

    constexpr size_t size() const { return data_.size(); }

    constexpr auto begin() const { return data_.begin(); }
    constexpr auto begin()       { return data_.begin(); }

    constexpr auto end() const { return data_.end(); }
    constexpr auto end()       { return data_.end(); }

private:

    static constexpr Key NULL_INDEX = std::numeric_limits<Key>::max();

    std::vector<Entry> data_; // dense
    // if the largest id used is huge the index array that maps key to value will use a lot of memory
    std::vector<Key> index_; // sparse

};


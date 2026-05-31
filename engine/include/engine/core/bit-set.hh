#pragma once
#include "engine/core/types.hh"
#include <cassert>
#include <vector>

// Similar to std::bitset but size doesn't need to be known at compile time.
struct BitSet {

    explicit BitSet(u32 size = 0u, bool fill = false)
        : words_((size + 63u) / 64u, (u64)fill)
        //: bytes_(size/64u+1u, 0u)
        , size_(size)
    {}

    BitSet(const BitSet &)                = default;
    BitSet(BitSet &&) noexcept            = default;
    BitSet &operator=(const BitSet &)     = default;
    BitSet &operator=(BitSet &&) noexcept = default;

    constexpr void set(u32 bit, bool value = true) {
        assert(bit < size_);
        if (value) { words_[bit / 64u] |=  (1ull << (bit % 64u)); ++count_; }
        else       { words_[bit / 64u] &= ~(1ull << (bit % 64u)); --count_; }
    }

    constexpr bool get(u32 bit) const {
        assert(bit < size_);
        return (words_[bit / 64u] >> (bit % 64u)) & 1ull;
    }

    constexpr bool any() const { return !none(); }

    constexpr bool none() const { return count_ == 0u; }

    // Returns the number of bits set to true.
    constexpr u32 count() const { return count_; }

    void reset() { std::fill(words_.begin(), words_.end(), 0ull); }

    constexpr u32 size() const { return size_; }

    BitSet &operator|=(const BitSet &other) {
        assert(this != &other);
        assert(size_ == other.size_);
        for (u32 i = 0u; i < size_; ++i)
            words_[i] |= other.words_[i];
        return *this;
    }

    BitSet operator|(const BitSet &other) const {
        assert(this != &other);
        return BitSet(*this) |= other;
    }

    BitSet &operator&=(const BitSet &other) {
        assert(this != &other);
        assert(size_ == other.size_);
        for (u32 i = 0u; i < size_; ++i)
            words_[i] &= other.words_[i];
        return *this;
    }

    BitSet operator&(const BitSet &other) const {
        assert(this != &other);
        return BitSet(*this) &= other;
    }

    BitSet &operator^=(const BitSet &other) {
        assert(this != &other);
        assert(size_ == other.size_);
        for (u32 i = 0u; i < size_; ++i)
            words_[i] ^= other.words_[i];
        return *this;
    }

    BitSet operator^(const BitSet &other) const {
        assert(this != &other);
        return BitSet(*this) ^= other;
    }

    // Are all flipped bits in this set also flipped in the other set?
    // Assumes: The bitsets are the same size.
    bool operator==(const BitSet &other) const {
        assert(this != &other);
        assert(size_ == other.size_);
        return count_ == other.count_
            && words_ == other.words_;
    }

    bool operator!=(const BitSet &other) const { return !(*this == other); }

private:

    std::vector<u64> words_; // u64 allows quicker comparison then u8
    u32 size_;
    u32 count_; // allows for quicker comparison

};


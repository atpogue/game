#pragma once
#include "engine/core/error.hh"
#include "engine/core/types.hh"

#include <vector>

// Similar to std::bitset but size doesn't need to be known at compile time.
struct BitSet
{

  explicit BitSet(u32 size = 0u, bool fill = false)
    : words_((size + 63u) / 64u, (u64)fill)
    //: bytes_(size/64u+1u, 0u)
    , size_(size)
  {
  }

  BitSet(const BitSet&)                = default;
  BitSet(BitSet&&) noexcept            = default;
  BitSet& operator=(const BitSet&)     = default;
  BitSet& operator=(BitSet&&) noexcept = default;
  ~BitSet() noexcept                   = default;

  void set(u32 idx, bool value = true) noexcept
  {
    PRECONDITION(idx < size_);
    if (value) {
      words_[idx / 64u] |= (1ull << (idx % 64u));
      ++count_;
    } else {
      words_[idx / 64u] &= ~(1ull << (idx % 64u));
      --count_;
    }
  }

  bool get(u32 idx) const noexcept
  {
    PRECONDITION(idx < size_);
    return (words_[idx / 64u] >> (idx % 64u)) & 1ull;
  }

  bool any() const noexcept { return !none(); }

  bool none() const noexcept { return count_ == 0u; }

  // Returns the number of bits set to true.
  u32 count() const noexcept { return count_; }

  void fill(bool value) { std::fill(words_.begin(), words_.end(), value ? UINT64_MAX : 0ull); }

  u32 size() const noexcept { return size_; }

  BitSet& operator|=(const BitSet& other) noexcept
  {
    PRECONDITION(this != &other);
    PRECONDITION(size_ == other.size_);
    for (u32 i = 0u; i < size_; ++i) words_[i] |= other.words_[i];
    return *this;
  }

  BitSet operator|(const BitSet& other) const noexcept
  {
    PRECONDITION(this != &other);
    return BitSet(*this) |= other;
  }

  BitSet& operator&=(const BitSet& other) noexcept
  {
    PRECONDITION(this != &other);
    PRECONDITION(size_ == other.size_);
    for (u32 i = 0u; i < size_; ++i) words_[i] &= other.words_[i];
    return *this;
  }

  BitSet operator&(const BitSet& other) const noexcept
  {
    PRECONDITION(this != &other);
    return BitSet(*this) &= other;
  }

  BitSet& operator^=(const BitSet& other) noexcept
  {
    PRECONDITION(this != &other);
    PRECONDITION(size_ == other.size_);
    for (u32 i = 0u; i < size_; ++i) words_[i] ^= other.words_[i];
    return *this;
  }

  BitSet operator^(const BitSet& other) const noexcept
  {
    PRECONDITION(this != &other);
    return BitSet(*this) ^= other;
  }

  // Are all flipped bits in this set also flipped in the other set?
  // Assumes: The bitsets are the same size.
  bool operator==(const BitSet& other) const noexcept
  {
    PRECONDITION(this != &other);
    PRECONDITION(size_ == other.size_);
    return count_ == other.count_ && words_ == other.words_;
  }

  bool operator!=(const BitSet& other) const noexcept { return !(*this == other); }

private:

  std::vector<u64> words_; // u64 allows quicker comparison then u8
  u32              size_;
  u32              count_; // allows for quicker comparison
};


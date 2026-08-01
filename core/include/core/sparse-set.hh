#pragma once
#include "core/paged-array.hh"
#include "core/panic.hh"
#include "core/types.hh"
#include <concepts>
#include <span>
#include <utility>
#include <vector>

template <typename Type>
struct SparseSet
{
  using size_type       = u32;
  using value_type      = Type;
  using reference       = Type&;
  using const_reference = Type const&;
  using pointer         = Type*;
  using const_pointer   = Type const*;

  explicit SparseSet(u32 capacity) : SparseSet() { reserve(capacity); }

  SparseSet()                                = default;
  SparseSet(SparseSet&&) noexcept            = default;
  SparseSet(SparseSet const&)                = default;
  SparseSet& operator=(SparseSet&&) noexcept = default;
  SparseSet& operator=(SparseSet const&)     = default;
  ~SparseSet() noexcept                      = default;

  [[nodiscard]] size_t capacity() const noexcept { return values_.capacity(); }

  void reserve(u32 count)
  {
    keys_.reserve(count);
    values_.reserve(count);
  }

  [[nodiscard]] bool has(u32 key) const noexcept { return lookup_.has(key); }

  // assumptions: key is not nil, key doesn't already exist
  template <typename... Args>
  requires std::constructible_from<Type, Args...>
  reference emplace(u32 key, Args&&... args)
  {
    PRECONDITION(key != nil);
    PRECONDITION(!has(key));
    INVARIANT(keys_.size() == values_.size(), "key and value dense arrays must be the same size");
    lookup_.emplace(key, keys_.size());
    keys_.push_back(key);
    return values_.emplace_back(std::forward<Args>(args)...);
  }

  // assumptions: key is not nil, key is assigned to an element
  void erase(u32 key) noexcept(std::is_nothrow_move_assignable_v<Type>)
    requires (std::is_move_assignable_v<Type> || std::is_copy_assignable_v<Type>)
  {
    PRECONDITION(key != nil);
    PRECONDITION(has(key));
    INVARIANT(keys_.size() == values_.size(), "key and value dense arrays must be the same size");
    // swap the indexes of the given key and the key at the end
    if (u32 end = keys_.back(); key != end) {
      u32 idx      = lookup_[key];
      lookup_[end] = idx;
      keys_[idx]   = end;
      values_[idx] = std::move(values_.back());
    }
    lookup_.erase(key);
    keys_.pop_back();
    values_.pop_back();
  }

  void clear() noexcept
  {
    values_.clear();
    keys_.clear();
    lookup_.clear();
  }

  [[nodiscard]] const_reference operator[](u32 key) const noexcept
  {
    DEBUG_ASSERT(has(key));
    return values_[lookup_[key]];
  }

  [[nodiscard]] reference operator[](u32 key) noexcept
  {
    DEBUG_ASSERT(has(key));
    return values_[lookup_[key]];
  }

  [[nodiscard]] const_pointer try_get(u32 key) const noexcept
  {
    return has(key) ? &values_[lookup_[key]] : nullptr;
  }

  [[nodiscard]] pointer try_get(u32 key) noexcept
  {
    return has(key) ? &values_[lookup_[key]] : nullptr;
  }

  [[nodiscard]] u32 size() const noexcept { return values_.size(); }

  std::span<u32 const> keys() const noexcept { return keys_; }

  std::span<Type> values() { return values_; }

  std::span<Type const> values() const { return values_; }

private:

  template <bool IsConst>
  struct Iterator
  { ////////////////////////////////////////////////////////////////////
    using Owner         = std::conditional_t<IsConst, SparseSet const, SparseSet>;
    using ReferenceType = std::conditional_t<IsConst, Type const&, Type&>;

  public:

    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void; // proxy; no operator-> provided
    using value_type        = std::pair<u32, ReferenceType>;
    using reference         = value_type;

    Iterator() : owner_{nullptr}, idx_{nil} {}

    // Implicit conversion from iterator to const_iterator.
    operator Iterator<true>() const noexcept requires (!IsConst)
    {
      return Iterator<true>(owner_, idx_);
    }

    [[nodiscard]] reference operator*() const noexcept
    {
      return {owner_->keys_[idx_], owner_->values_[idx_]};
    }

    Iterator& operator++() noexcept
    {
      ++idx_;
      return *this;
    }

    Iterator operator++(int) noexcept
    {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(Iterator const& other) const noexcept
    {
      return owner_ == other.owner_ && idx_ == other.idx_;
    }

    bool operator!=(Iterator const& other) const noexcept { return !(*this == other); }

  private:

    friend struct SparseSet<Type>;

    Iterator(Owner* owner, u32 idx) noexcept : owner_{owner}, idx_{idx} {}

    Owner* owner_;
    u32    idx_;
  }; //////////////////////////////////////////////////////////////////////////////////

public:

  using iterator       = Iterator<false>;
  using const_iterator = Iterator<true>;

  [[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator(this, 0u); }

  [[nodiscard]] const_iterator cend() const noexcept { return const_iterator(this, size()); }

  [[nodiscard]] iterator begin() noexcept { return iterator(this, 0u); }

  [[nodiscard]] iterator end() noexcept { return iterator(this, size()); }

  [[nodiscard]] const_iterator begin() const noexcept { return cbegin(); }

  [[nodiscard]] const_iterator end() const noexcept { return cend(); }

private:

  // Invariants:
  // - values_ and keys_ and lookup_ are all the same size
  // - keys_[lookup_[key]] equals key

  std::vector<Type>    values_;
  std::vector<u32>     keys_;
  PagedArray<u32, 256> lookup_; // 256 * 4B = 1 KB of memory per page when u32 is 32-bit
};

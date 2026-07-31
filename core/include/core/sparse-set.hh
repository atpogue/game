#pragma once
#include "core/paged-array.hh"
#include "core/panic.hh"
#include "core/types.hh"
#include <concepts>
#include <span>
#include <utility>
#include <vector>

template <typename Type, std::unsigned_integral Key = u32>
struct SparseSet
{
  using size_type       = Key;
  using value_type      = Type;
  using reference       = Type&;
  using const_reference = Type const&;
  using pointer         = Type*;
  using const_pointer   = Type const*;

  explicit SparseSet(Key capacity) : SparseSet() { reserve(capacity); }

  SparseSet()                                = default;
  SparseSet(SparseSet&&) noexcept            = default;
  SparseSet(SparseSet const&)                = default;
  SparseSet& operator=(SparseSet&&) noexcept = default;
  SparseSet& operator=(SparseSet const&)     = default;
  ~SparseSet() noexcept                      = default;

  [[nodiscard]] size_t capacity() const noexcept { return values_.capacity(); }

  void reserve(Key count)
  {
    keys_.reserve(count);
    values_.reserve(count);
  }

  [[nodiscard]] bool has(Key key) const noexcept { return lookup_.has(key); }

  // assumptions: key is not nil, key doesn't already exist
  template <typename... Args>
  requires std::constructible_from<Type, Args...>
  reference emplace(Key key, Args&&... args)
  {
    PRECONDITION(key != nil);
    PRECONDITION(!has(key));
    INVARIANT(keys_.size() == values_.size(), "key and value dense arrays must be the same size");
    lookup_.emplace(key, keys_.size());
    keys_.push_back(key);
    return values_.emplace_back(std::forward<Args>(args)...);
  }

  // assumptions: key is not nil, key is assigned to an element
  void erase(Key key) noexcept(std::is_nothrow_move_assignable_v<Type>)
    requires (std::is_move_assignable_v<Type> || std::is_copy_assignable_v<Type>)
  {
    PRECONDITION(key != nil);
    PRECONDITION(has(key));
    INVARIANT(keys_.size() == values_.size(), "key and value dense arrays must be the same size");
    // swap the indexes of the given key and the key at the end
    if (Key end = keys_.back(); key != end) {
      Key idx      = lookup_[key];
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

  [[nodiscard]] const_reference operator[](Key key) const noexcept
  {
    DEBUG_ASSERT(has(key));
    return values_[lookup_[key]];
  }

  [[nodiscard]] reference operator[](Key key) noexcept
  {
    DEBUG_ASSERT(has(key));
    return values_[lookup_[key]];
  }

  [[nodiscard]] const_pointer try_get(Key key) const noexcept
  {
    return has(key) ? &values_[lookup_[key]] : nullptr;
  }

  [[nodiscard]] pointer try_get(Key key) noexcept
  {
    return has(key) ? &values_[lookup_[key]] : nullptr;
  }

  [[nodiscard]] Key size() const noexcept { return values_.size(); }

  std::span<Key> keys() const noexcept { return keys_; }

  std::span<Type> values() { return values_; }

  std::span<Type const> values() const { return values_; }

private:

  template <bool IsConst>
  struct Iterator
  { ////////////////////////////////////////////////////////////////////
    using OwnerPtr      = std::conditional_t<IsConst, SparseSet const*, SparseSet*>;
    using ReferenceType = std::conditional_t<IsConst, Type const&, Type&>;

  public:

    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void; // proxy; no operator-> provided
    using value_type        = std::pair<Key, ReferenceType>;
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

    Iterator(OwnerPtr owner, Key idx) noexcept : owner_{owner}, idx_{idx} {}

    OwnerPtr owner_;
    Key      idx_;
  }; //////////////////////////////////////////////////////////////////////////////////

public:

  using iterator       = Iterator<false>;
  using const_iterator = Iterator<true>;

  [[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator(this, 0u); }

  [[nodiscard]] const_iterator begin() const noexcept { return cbegin(); }

  [[nodiscard]] iterator begin() noexcept { return iterator(this, 0u); }

  [[nodiscard]] const_iterator cend() const noexcept
  {
    return const_iterator(this, values_.size());
  }

  [[nodiscard]] const_iterator end() const noexcept { return cend(); }

  [[nodiscard]] iterator end() noexcept { return iterator(this, values_.size()); }

private:

  // Invariants:
  // - values_ and keys_ and lookup_ are all the same size
  // - keys_[lookup_[key]] equals key

  std::vector<Type>    values_;
  std::vector<Key>     keys_;
  PagedArray<Key, 256> lookup_; // 256 * 4B = 1 KB of memory per page when Key is 32-bit
};

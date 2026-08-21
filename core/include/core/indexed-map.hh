#pragma once
#include "core/panic.hh"
#include "core/types.hh"
#include <algorithm>
#include <span>
#include <vector>

// An append-only, densely packed key/value store whose elements can be accessed directly via a
// stable numerical index (the append order).
template <typename Key, typename Type, typename Compare = std::less<>>
struct IndexedMap
{
  using size_type  = u32;
  using key_type   = Key;
  using value_type = Type;
  using reference  = Type&;
  using pointer    = Type*;

  explicit IndexedMap(u32 capacity) : IndexedMap() { reserve(capacity); }

  IndexedMap()                                 = default;
  IndexedMap(IndexedMap&&) noexcept            = default;
  IndexedMap(IndexedMap const&)                = default;
  IndexedMap& operator=(IndexedMap&&) noexcept = default;
  IndexedMap& operator=(IndexedMap const&)     = default;
  ~IndexedMap() noexcept                       = default;

  // Return an ID that is assigned according to append order.
  // Assumes: key doesn't already exist, maximum size not reached.
  template <typename K, typename... Args>
  requires std::constructible_from<Key, K> && std::constructible_from<Type, Args...>
  u32 emplace(K&& key, Args&&... args)
  {
    ASSERT(keys_.size() < nil, "maximum size reached");
    INVARIANT(keys_.size() == values_.size() && keys_.size() == sorted_.size());
    auto pos = locate(key);
    PRECONDITION(!found(pos, key), "key must not be assigned to more than one element");
    u32 const idx = static_cast<u32>(keys_.size());
    keys_.emplace_back(std::forward<K>(key));
    values_.emplace_back(std::forward<Args>(args)...);
    sorted_.insert(pos, idx);
    return idx;
  }

  template <typename K>
  requires std::constructible_from<Key, K>
  [[nodiscard]] bool has(K const& key) const
  {
    return found(locate(key), key);
  }

  // Performs O(logN) binary search to find the ID of the element with the given key.
  // If the key isn't present, returns nil.
  template <typename K>
  requires std::constructible_from<Key, K>
  [[nodiscard]] u32 find(K const& key) const
  {
    auto pos = locate(key);
    return found(pos, key) ? *pos : nil;
  }

  // Returns null if no element has the given key.
  template <typename K>
  requires std::constructible_from<Key, K>
  [[nodiscard]] Type const* try_get(K const& key) const
  {
    u32 const idx = find(key);
    return idx != nil ? &values_[idx] : nullptr;
  }

  // Returns null if no element has the given key.
  template <typename K>
  requires std::constructible_from<Key, K>
  [[nodiscard]] Type* try_get(K const& key)
  {
    u32 const idx = find(key);
    return idx != nil ? &values_[idx] : nullptr;
  }

  // O(1) direct access.
  [[nodiscard]] Type const& operator[](u32 idx) const noexcept
  {
    DEBUG_ASSERT(idx < size());
    return values_[idx];
  }

  // O(1) direct access.
  [[nodiscard]] Type& operator[](u32 idx) noexcept
  {
    DEBUG_ASSERT(idx < size());
    return values_[idx];
  }

  [[nodiscard]] Key const& key(u32 idx) const noexcept
  {
    DEBUG_ASSERT(idx < size());
    return keys_[idx];
  }

  [[nodiscard]] u32 size() const noexcept { return static_cast<u32>(values_.size()); }

  [[nodiscard]] size_t capacity() const noexcept { return values_.capacity(); }

  void reserve(u32 n)
  {
    keys_.reserve(n);
    values_.reserve(n);
    sorted_.reserve(n);
  }

  void clear() noexcept
  {
    keys_.clear();
    values_.clear();
    sorted_.clear();
  }

  [[nodiscard]] std::span<Key const> keys() const noexcept { return keys_; }

  [[nodiscard]] std::span<Type> values() noexcept { return values_; }

  [[nodiscard]] std::span<Type const> values() const noexcept { return values_; }

private:

  template <AccessFlag Access>
  struct Iterator
  { ////////////////////////////////////////////////////////////////////
    using Owner         = std::conditional_t<Access == Write, IndexedMap, IndexedMap const>;
    using ReferenceType = std::conditional_t<Access == Write, Type&, Type const&>;

  public:

    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void; // proxy; no operator-> provided
    using value_type        = std::pair<Key, ReferenceType>;
    using reference         = value_type;

    Iterator() : owner_{ nullptr }, idx_{ nil } {}

    // Implicit conversion from iterator to const_iterator.
    operator Iterator<Read>() const noexcept requires (Access == Write)
    {
      return Iterator<Read>(owner_, idx_);
    }

    [[nodiscard]] reference operator*() const noexcept
    {
      return { owner_->keys_[idx_], owner_->values_[idx_] };
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

    friend struct IndexedMap<Key, Type, Compare>;

    Iterator(Owner* owner, u32 idx) noexcept : owner_{ owner }, idx_{ idx } {}

    Owner* owner_;
    u32    idx_;
  }; //////////////////////////////////////////////////////////////////////////////////

public:

  using iterator       = Iterator<Write>;
  using const_iterator = Iterator<Read>;

  [[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator(this, 0u); }

  [[nodiscard]] const_iterator cend() const noexcept { return const_iterator(this, size()); }

  [[nodiscard]] iterator begin() noexcept { return iterator(this, 0u); }

  [[nodiscard]] iterator end() noexcept { return iterator(this, size()); }

  [[nodiscard]] const_iterator begin() const noexcept { return cbegin(); }

  [[nodiscard]] const_iterator end() const noexcept { return cend(); }

private:

  // Locates the key in the sorted array.
  // If not found, returns an iterator at which the key could be inserted to maintain order.
  template <typename K>
  [[nodiscard]] std::vector<u32>::const_iterator locate(K const& key) const
  {
    return std::lower_bound(sorted_.begin(), sorted_.end(), key, [this](u32 idx, K const& k) {
      return compare_(keys_[idx], k);
    });
  }

  // Does the sorted iterator returned by `locate` hold an equal key?
  template <typename K>
  [[nodiscard]] bool found(std::vector<u32>::const_iterator pos, K const& key) const
  {
    // lower_bound guarantees !(keys_[sorted_[pos]] < key), so equality reduces
    // to !(key < keys_[sorted_[pos]]).
    return pos != sorted_.end() && !compare_(key, keys_[*pos]);
  }

  std::vector<Key>  keys_;
  std::vector<Type> values_;

  // Contains the append-order indexes, is sorted by key.
  // Using a sorted vector instead of a map to avoid storing the keys twice.
  std::vector<u32>              sorted_;
  [[no_unique_address]] Compare compare_;

  // Invariants:
  // - keys_, values_, sorted_ are all the same size

  // Constraints:
  // - Keys are immutable after insertion: mutation would silently corrupt the append-order index.
  // - Erasing breaks index stability; tombstones would preserve it at the cost of density.
  // - Inserting breaks index stability.

  // Deferred:
  // - Bulk Load: Sorting could be deferred until after all elements are appended, making N
  // insertions O(N * logN) total. Currently, emplace is has an O(N) worst-case insert into the
  // sorted array, but the sorted array is populated by indexes which are trivial to move around.
};


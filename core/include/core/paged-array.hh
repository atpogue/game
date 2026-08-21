#pragma once

// TODO: PagedArray::merge(PagedArray&&) that moves values defined in the other array into this one
// without modifying the values the other array doesn't have

#include "core/panic.hh"
#include "core/types.hh"
#include <bit>
#include <bitset>
#include <memory>
#include <new>
#include <vector>

// A paged, dynamically allocated sparse array with O(1) insert, erase, lookup.
// Only pages that contain at least one element are allocated. Empty pages are
// destroyed on erase. Useful anywhere you want access to arbitrary indices
// without committing to a single large flat allocation.
template <typename Type, u32 PageSize>
requires (PageSize > 1u) && (std::has_single_bit(PageSize))
struct PagedArray
{
  using size_type       = u32;
  using value_type      = Type;
  using reference       = Type&;
  using const_reference = Type const&;
  using pointer         = Type*;
  using const_pointer   = Type const*;

private:

  struct Page
  { ///////////////////////////////////////////////////////////////////////
    Page()                       = default;
    Page(Page const&)            = delete;
    Page& operator=(Page const&) = delete;

    Type* at(u32 i) noexcept { return std::launder(reinterpret_cast<Type*>(storage) + i); }

    Type const* at(u32 i) const noexcept
    {
      return std::launder(reinterpret_cast<Type const*>(storage) + i);
    }

    ~Page() noexcept
    {
      if constexpr (!std::is_trivially_destructible_v<Type>) { // constexpr if
                                                               // statement?
        for (u32 i = 0u; i < PageSize; ++i)
          if (occupied[i]) std::destroy_at(at(i));
      }
    }

    alignas(Type) std::byte storage[PageSize * sizeof(Type)];
    std::bitset<PageSize> occupied;
    u32                   count = 0u;
  }; //////////////////////////////////////////////////////////////////////////////////

  static void copy_page(Page const& src, Page& dst)
  {
    dst.occupied = src.occupied;
    dst.count    = src.count;
    for (u32 j = 0; j < PageSize; ++j)
      if (src.occupied[j]) std::construct_at(dst.at(j), *src.at(j));
  }

  static constexpr u32 shift = std::bit_width(PageSize) - 1; // log2(PageSize)
  static constexpr u32 mask  = PageSize - 1;

  static constexpr u32 index_of(u32 page, u32 slot) { return (page << shift) | slot; }

  static constexpr u32 page_of(u32 idx) noexcept { return idx >> shift; }

  static constexpr u32 slot_of(u32 idx) noexcept { return idx & mask; }

public:

  PagedArray()           = default;
  ~PagedArray() noexcept = default;

  PagedArray(PagedArray const& other) requires std::is_copy_constructible_v<Type>
    : pages_(), size_{ other.size_ }
  {
    pages_.resize(other.pages_.size());
    for (u32 i = 0; i < other.pages_.size(); ++i) {
      if (!other.pages_[i]) continue;
      pages_[i] = std::make_unique<Page>();
      copy_page(*other.pages_[i], *pages_[i]);
    }
  }

  PagedArray& operator=(PagedArray const& other) requires std::is_copy_constructible_v<Type>
  {
    if (this == &other) return *this;
    pages_.clear();
    pages_.resize(other.pages_.size());
    for (u32 i = 0; i < other.pages_.size(); ++i) {
      pages_[i] = std::make_unique<Page>();
      copy_page(*other.pages_[i], *pages_[i]);
    }
    return *this;
  }

  // Note: is noexcept without requiring the Type to be move constructible
  PagedArray(PagedArray&& other) noexcept : pages_(std::move(other.pages_)), size_(other.size_)
  {
    other.size_ = 0u;
  }

  // Note: is noexcept without requiring the Type to be move constructible
  PagedArray& operator=(PagedArray&& other) noexcept
  {
    if (this == &other) return *this;
    pages_      = std::move(other.pages_);
    size_       = other.size_;
    other.size_ = 0u;
    return *this;
  }

  /// Number of occupied elements across all pages.
  [[nodiscard]] u32 size() const noexcept { return size_; }

  /// Not all pages may be allocated.
  [[nodiscard]] u32 page_count() const noexcept { return pages_.size(); }

  bool has(u32 idx) const noexcept
  {
    Page const* page = get_page(idx);
    return page && page->occupied[slot_of(idx)];
  }

  // Direct access to the element at the index without bounds-checking.
  // The validity of the index is asserted in debug mode, and assumed (zero-cost
  // overhead) in release mode.
  [[nodiscard]] reference operator[](u32 idx) noexcept
  {
    DEBUG_ASSERT(has(idx));
    return *get_page(idx)->at(slot_of(idx));
  }

  // Direct access to the element at the index without bounds-checking.
  // The validity of the index is asserted in debug mode, and assumed (zero-cost
  // overhead) in release mode.
  [[nodiscard]] const_reference operator[](u32 idx) const noexcept
  {
    DEBUG_ASSERT(has(idx));
    return *get_page(idx)->at(slot_of(idx));
  }

  [[nodiscard]] pointer get(u32 idx) noexcept
  {
    Page* page = get_page(idx);
    u32   s    = slot_of(idx);
    return (page && page->occupied[s]) ? page->at(s) : nullptr;
  }

  [[nodiscard]] const_pointer get(u32 idx) const noexcept
  {
    Page const* page = get_page(idx);
    u32         s    = slot_of(idx);
    return (page && page->occupied[s]) ? page->at(s) : nullptr;
  }

  /// Assumes: there is no element at [idx]
  template <typename... Args>
  reference emplace(u32 idx, Args&&... args)
  {
    PRECONDITION(idx != nil);
    Page& page = get_or_create_page(idx);
    u32   s    = slot_of(idx);
    PRECONDITION(!page.occupied[s]);
    page.occupied.set(s, true);
    ++page.count;
    ++size_;
    return *std::construct_at(page.at(s), std::forward<Args>(args)...);
  }

  /// Assumes: there is an element at [idx]
  void erase(u32 idx) noexcept
  {
    PRECONDITION(idx != nil);
    Page* page = get_page(idx);
    u32   s    = slot_of(idx);
    PRECONDITION(page && page->occupied[s], "index must be occupied");
    std::destroy_at(page->at(s));
    page->occupied.set(s, false);
    --page->count;
    --size_;
    if (page->count == 0) pages_[page_of(idx)].reset();
  }

  void clear() noexcept
  {
    pages_.clear();
    size_ = 0;
  }

private:

  // If the slot at the given index is empty, returns the index of the next
  // occupied slot. If an occupied slot wasn't found, returns nil. Else, if the
  // given slot is already occupied, returns the given index.
  constexpr u32 find_occupied(u32 idx) const noexcept
  {
    auto p = page_of(idx);
    auto s = slot_of(idx);
    while (p < pages_.size()) {
      auto const& page = pages_[p];
      if (page) {
        while (s < PageSize) {
          if (page->occupied[s]) return index_of(p, s);
          ++s;
        }
      }
      ++p;
      s = 0;
    }
    return nil;
  }

  // Any insert that causes pages_ to reallocate invalidates all iterators.
  // Erase only invalidates iterators to the erased element.
  template <AccessFlag Access>
  struct Iterator
  { ////////////////////////////////////////////////////////////////////
    using OwnerPtr      = std::conditional_t<Access == Write, PagedArray*, PagedArray const*>;
    using ReferenceType = std::conditional_t<Access == Write, Type&, Type const&>;
    using PageType      = std::conditional_t<Access == Write, Page, Page const>;

  public:

    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void; // proxy; no operator-> provided
    using value_type        = std::pair<u32, ReferenceType>;
    using reference         = value_type;

    Iterator() : owner_{ nullptr }, idx_{ nil } {}

    operator Iterator<Read>() const noexcept requires (Access == Write)
    {
      return Iterator<Read>(owner_, idx_);
    }

    value_type operator*() const noexcept
    {
      PRECONDITION(owner_, "dereference singular iterator");
      PRECONDITION(idx_ != nil, "dereferenced end iterator");
      INVARIANT(owner_->has(idx_), "dereferenced empty slot");
      return value_type{ idx_, (*owner_)[idx_] };
    }

    Iterator& operator++() noexcept
    {
      if (idx_ != nil) idx_ = owner_->find_occupied(++idx_);
      return *this;
    }

    Iterator operator++(int) noexcept
    {
      auto it = *this;
      ++(*this);
      return it;
    }

    bool operator==(Iterator const& other) const noexcept
    {
      return owner_ == other.owner_ && idx_ == other.idx_;
    }

    bool operator!=(Iterator const& other) const noexcept { return !(*this == other); }

  private:

    friend struct PagedArray<Type, PageSize>;

    Iterator(OwnerPtr owner, u32 idx) noexcept : owner_(owner), idx_{ idx }
    {
      INVARIANT(owner_, "constructed without parent container");
      idx_ = owner_->find_occupied(idx_);
    }

    OwnerPtr owner_;
    u32      idx_;
  }; //////////////////////////////////////////////////////////////////////////////////

public:

  using iterator       = Iterator<Write>;
  using const_iterator = Iterator<Read>;

  const_iterator cbegin() const noexcept { return const_iterator(this, 0u); }

  const_iterator cend() const noexcept { return const_iterator(this, nil); }

  iterator begin() noexcept { return iterator(this, 0u); }

  const_iterator begin() const noexcept { return cbegin(); }

  iterator end() noexcept { return iterator(this, nil); }

  const_iterator end() const noexcept { return cend(); }

private:

  Page* get_page(u32 idx) noexcept
  {
    u32 p = page_of(idx);
    return (p < pages_.size()) ? pages_[p].get() : nullptr;
  }

  Page const* get_page(u32 idx) const noexcept
  {
    u32 p = page_of(idx);
    return (p < pages_.size()) ? pages_[p].get() : nullptr;
  }

  Page& get_or_create_page(u32 idx)
  {
    u32 p = page_of(idx);
    if (p >= pages_.size()) pages_.resize(p + 1u);
    auto& page = pages_[p];
    if (!page) page = std::make_unique<Page>();
    return *page;
  }

  std::vector<std::unique_ptr<Page>> pages_;
  u32                                size_ = 0u;
};

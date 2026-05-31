#pragma once
#include "engine/core/types.hh"
#include <bit>
#include <bitset>
#include <cassert>
#include <memory>
#include <vector>

// A paged, dynamically allocated sparse array with O(1) insert, erase, lookup.
// Only pages that contain at least one element are allocated. Empty pages are destroyed on erase.
// Useful anywhere you want access to arbitrary indices without committing to a single large flat allocation.
template <typename Type, u32 PageSize>
requires (PageSize > 1u) && (std::has_single_bit(PageSize))
struct PagedArray {

    using size_type       = u32;
    using value_type      = Type;
    using reference       = Type &;
    using const_reference = const Type &;
    using pointer         = Type *;
    using const_pointer   = const Type *;

private:

    struct Page { ///////////////////////////////////////////////////////////////////////

        Page()                          = default;
        Page(const Page &)              = delete;
        Page &operator=(const Page &)   = delete;

              Type *at(u32 i)       noexcept { return std::launder(reinterpret_cast<      Type *>(storage) + i); }
        const Type *at(u32 i) const noexcept { return std::launder(reinterpret_cast<const Type *>(storage) + i); }

        ~Page() noexcept {
            if constexpr (!std::is_trivially_destructible_v<Type>) { // constexpr if statement?
                for (u32 i = 0u; i < PageSize; ++i)
                    if (occupied[i]) std::destroy_at(at(i));
            }
        }

        alignas(Type) std::byte storage[PageSize * sizeof(Type)];
        std::bitset<PageSize>   occupied;
        u32                     count = 0u;

    }; //////////////////////////////////////////////////////////////////////////////////

    static constexpr u32 shift = std::bit_width(PageSize) - 1; // log2(PageSize)
    static constexpr u32 mask  = PageSize - 1;

    static constexpr u32 index_of(u32 page, u32 slot) { return (page << shift) | slot; }
    static constexpr u32 page_of(u32 idx) noexcept { return idx >> shift; }
    static constexpr u32 slot_of(u32 idx) noexcept { return idx &  mask;  }

public:

    PagedArray()  = default;
    PagedArray &operator=(const PagedArray &) = delete;
    ~PagedArray() = default;

    // Note: is noexcept without requiring the Type to be move constructible
    PagedArray(PagedArray &&other) noexcept
        : pages_(std::move(other.pages_)), size_(other.size_)
    {
        other.size_ = 0u;
    }

    // Note: is noexcept without requiring the Type to be move constructible
    PagedArray &operator=(PagedArray &&other) noexcept {
        if (this == &other) return *this;
        pages_ = std::move(other.pages_);
        size_ = other.size_;
        other.size_ = 0u;
        return *this;
    }

    /// Number of occupied elements across all pages.
    [[nodiscard]] u32 size() const noexcept { return size_; }

    /// Not all pages may be allocated.
    [[nodiscard]] u32 page_count() const noexcept { return pages_.size(); }

    bool has(u32 idx) const noexcept {
        const Page *page = get_page(idx);
        return page && page->occupied[slot_of(idx)];
    }

    [[nodiscard]] reference operator[](u32 idx) noexcept {
        assert(has(idx));
        return *get_page(idx)->at(slot_of(idx));
    }

    [[nodiscard]] const_reference operator[](u32 idx) const noexcept {
        assert(has(idx));
        return *get_page(idx)->at(slot_of(idx));
    }

    [[nodiscard]] pointer get(u32 idx) noexcept {
        Page* page = get_page(idx);
        u32 s = slot_of(idx);
        return (page && page->occupied[s]) ? page->at(s) : nullptr;
    }

    [[nodiscard]] const_pointer get(u32 idx) const noexcept {
        const Page *page = get_page(idx);
        u32 s = slot_of(idx);
        return (page && page->occupied[s]) ? page->at(s) : nullptr;
    }

    /// Assumes: there is no element at [idx]
    template <typename... Args>
    reference emplace(u32 idx, Args&&... args) {
        assert(idx != nil && "emplace on nil index");
        Page &page = get_or_create_page(idx);
        u32 s = slot_of(idx);
        assert(!page.occupied[s] && "emplace on existing element");
        page.occupied.set(s, true);
        ++page.count;
        ++size_;
        return *std::construct_at(page.at(s), std::forward<Args>(args)...);
    }

    /// Assumes: there is an element at [idx]
    void erase(u32 idx) noexcept {
        assert(idx != nil && "erase on nil index");
        Page *page = get_page(idx);
        u32 s = slot_of(idx);
        assert(page && page->occupied[s] && "erase on non-existing element");
        std::destroy_at(page->at(s));
        page->occupied.set(s, false);
        --page->count;
        --size_;
        if (page->count == 0) pages_[page_of(idx)].reset();
    }

    void clear() noexcept { pages_.clear(); size_ = 0; }

    [[nodiscard]] PagedArray copy() const requires std::is_copy_constructible_v<Type> { return *this; }

private:

    // If the slot at the given index is empty, returns the index of the next occupied slot.
    // If an occupied slot wasn't found, returns nil.
    // Else, if the given slot is already occupied, returns the given index.
    constexpr u32 find_occupied(u32 idx) const noexcept {
        auto p = page_of(idx);
        auto s = slot_of(idx);
        while (p < pages_.size()) {
            const auto &page = pages_[p];
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
    template <bool IsConst>
    struct Iterator { ////////////////////////////////////////////////////////////////////

        using OwnerPtr      = std::conditional_t<IsConst, const PagedArray *, PagedArray *>;
        using ReferenceType = std::conditional_t<IsConst, const Type &, Type &>;
        using PageType      = std::conditional_t<IsConst, const Page, Page>;

    public:

        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void; // proxy; no operator-> provided
        using value_type        = std::pair<u32, ReferenceType>;
        using reference         = value_type;

        Iterator() : owner_{nullptr}, idx_{nil} {}

        operator Iterator<true>() const noexcept requires (!IsConst) {
            return Iterator<true>(owner_, idx_);
        }

        value_type operator*() const noexcept {
            assert(owner_ && "dereferenced singular iterator");
            assert(idx_ != nil && "dereferenced end iterator");
            assert(owner_->has(idx_) && "dereferenced empty slot");
            return value_type{idx_, (*owner_)[idx_]};
        }

        Iterator &operator++() noexcept {
            if (idx_ != nil) idx_ = owner_->find_occupied(++idx_);
            return *this;
        }

        Iterator operator++(int) noexcept { auto it = *this; ++(*this); return it; }

        bool operator==(const Iterator& other) const noexcept {
            return owner_ == other.owner_ && idx_ == other.idx_;
        }

        bool operator!=(const Iterator& other) const noexcept { return !(*this == other); }
    
    private:

        friend struct PagedArray<Type, PageSize>;

        Iterator(OwnerPtr owner, u32 idx) noexcept
            : owner_(owner), idx_{idx}
        {
            assert(owner_ && "constructed without parent container");
            idx_ = owner_->find_occupied(idx_);
        }

        OwnerPtr owner_;
        u32 idx_;

    }; //////////////////////////////////////////////////////////////////////////////////

public:

    using iterator       = Iterator<false>;
    using const_iterator = Iterator<true>;

    const_iterator cbegin() const noexcept { return const_iterator(this, 0u); }
    const_iterator cend()   const noexcept { return const_iterator(this, nil); }

    iterator       begin()       noexcept { return iterator(this, 0u); }
    const_iterator begin() const noexcept { return cbegin(); }

    iterator       end()       noexcept { return iterator(this, nil); }
    const_iterator end() const noexcept { return cend(); }

private:

    PagedArray(const PagedArray &other)
    requires std::is_copy_constructible_v<Type>
    {
        pages_.resize(other.pages_.size());
        for (u32 i = 0; i < other.pages_.size(); ++i) {
            if (!other.pages_[i]) continue;
            const Page &src = *other.pages_[i];
            pages_[i] = std::make_unique<Page>();
            Page &dst  = *pages_[i];
            dst.occupied = src.occupied;
            dst.count    = src.count;
            for (u32 j = 0; j < PageSize; ++j)
                if (src.occupied[j]) std::construct_at(dst.at(j), *src.at(j));
        }
        size_ = other.size_;
    }

    Page *get_page(u32 idx) noexcept {
        u32 p = page_of(idx);
        return (p < pages_.size()) ? pages_[p].get() : nullptr;
    }

    const Page *get_page(u32 idx) const noexcept {
        u32 p = page_of(idx);
        return (p < pages_.size()) ? pages_[p].get() : nullptr;
    }

    Page &get_or_create_page(u32 idx) {
        u32 p = page_of(idx);
        if (p >= pages_.size()) pages_.resize(p + 1u);
        auto &page = pages_[p];
        if (!page) page = std::make_unique<Page>();
        return *page;
    }

    std::vector<std::unique_ptr<Page>> pages_;
    u32 size_ = 0;

};


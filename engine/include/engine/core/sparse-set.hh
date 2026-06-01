#pragma once
#include "engine/core/error.hh"
#include "engine/core/types.hh"
#include "engine/core/paged-array.hh"
#include <utility>
#include <concepts>
#include <vector>

template <typename Type>
struct SparseSet {

    using size_type       = u32;
    using value_type      = Type;
    using reference       = Type &;
    using const_reference = const Type &;
    using pointer         = Type *;
    using const_pointer   = const Type *;

    explicit SparseSet(u32 capacity)
        : SparseSet()
    {
        reserve(capacity);
    }

    SparseSet() = default;
    SparseSet(SparseSet &&) noexcept = default;
    SparseSet &operator=(SparseSet &&) noexcept = default;
    SparseSet &operator=(const SparseSet &) = delete;
    ~SparseSet() noexcept = default;

    [[nodiscard]] size_t capacity() const noexcept { return values_.capacity(); }

    void reserve(u32 count) { keys_.reserve(count); values_.reserve(count); }

    [[nodiscard]] bool has(u32 key) const noexcept { return lookup_.has(key); }

    // assumptions: key is not nil, key doesn't already exist
    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    reference emplace(u32 key, Args &&... args) {
        PRECONDITION(key != nil);
        PRECONDITION(!has(key));
        INVARIANT(keys_.size() == values_.size(), "key and value dense arrays must be the same size");
        lookup_.emplace(key, keys_.size());
        keys_.push_back(key);
        return values_.emplace_back(std::forward<Args>(args)...);
    }

    // assumptions: key is not nil, key is assigned to an element
    void erase(u32 key) noexcept(std::is_nothrow_move_assignable_v<Type>) {
        PRECONDITION(key != nil);
        PRECONDITION(has(key));
        INVARIANT(keys_.size() == values_.size(), "key and value dense arrays must be the same size");

        // swap the indexes of the given key and the key at the end
        if (u32 end = keys_.back(); key != end) {
            u32 idx = lookup_[key];
            lookup_[end] = idx;
            keys_[idx] = end;
            values_[idx] = std::move(values_.back());
        }

        lookup_[key] = nil;
        keys_.pop_back();
        values_.pop_back();
    }

    void clear() noexcept {
        values_.clear(); keys_.clear(); lookup_.clear();
    }

    [[nodiscard]] const_reference operator[](u32 key) const noexcept {
        ASSERT(has(key));
        return values_[lookup_[key]];
    }

    [[nodiscard]] reference operator[](u32 key) noexcept {
        ASSERT(has(key));
        return values_[lookup_[key]];
    }

    [[nodiscard]] const_pointer get(u32 key) const noexcept { return has(key) ? &values_[lookup_[key]] : nullptr; }
    [[nodiscard]]       pointer get(u32 key)       noexcept { return has(key) ? &values_[lookup_[key]] : nullptr; }

    [[nodiscard]] u32 size() const noexcept { return values_.size(); }

    [[nodiscard]] SparseSet copy() const requires std::is_copy_constructible_v<Type> { return *this; }

    const auto &keys() const noexcept { return keys_; }

          auto &values()       noexcept { return values_; }
    const auto &values() const noexcept { return values_; }

private:

    template <bool IsConst>
    struct Iterator { ////////////////////////////////////////////////////////////////////

        using OwnerPtr      = std::conditional_t<IsConst, const SparseSet *, SparseSet *>;
        using ReferenceType = std::conditional_t<IsConst, const Type &, Type &>;

    public:

        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void; // proxy; no operator-> provided
        using value_type        = std::pair<u32, ReferenceType>;
        using reference         = value_type;

        Iterator() : owner_{nullptr}, idx_{nil} {}

        // Implicit conversion from iterator to const_iterator.
        operator Iterator<true>() const noexcept requires (!IsConst) {
            return Iterator<true>(owner_, idx_);
        }

        [[nodiscard]] value_type operator*() const noexcept {
            return { owner_->keys_[idx_], owner_->values_[idx_] };
        }

        Iterator &operator++() noexcept { ++idx_; return *this; }

        Iterator operator++(int) noexcept { auto tmp = *this; ++(*this); return tmp; }

        bool operator==(const Iterator& other) const noexcept {
            return owner_ == other.owner_ && idx_ = other.idx_;
        }

        bool operator!=(const Iterator& other) const noexcept { return !(*this == other); }
    
    private:

        friend struct SparseSet<Type>;

        Iterator(OwnerPtr owner, u32 idx) noexcept
            : owner_{owner}, idx_{idx}
        {}

        OwnerPtr owner_;
        u32 idx_;

    }; //////////////////////////////////////////////////////////////////////////////////

public:

    using iterator       = Iterator<false>;
    using const_iterator = Iterator<true>;

    [[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator(this, 0u); }
    [[nodiscard]] const_iterator begin() const noexcept { return cbegin(); }
    [[nodiscard]] iterator       begin()       noexcept { return iterator(this, 0u); }

    [[nodiscard]] const_iterator cend() const noexcept { return const_iterator(this, values_.size()); }
    [[nodiscard]] const_iterator end() const noexcept { return cend(); }
    [[nodiscard]] iterator       end()       noexcept { return iterator(this, values_.size()); }

private:

    SparseSet(const SparseSet &) requires std::is_copy_constructible_v<Type> = default;

    // Invariants:
    // - values_ and keys_ and lookup_ are all the same size
    // - keys_[lookup_[key]] equals key

    std::vector<Type> values_;
    std::vector<u32> keys_;
    PagedArray<u32, 256> lookup_; // 256 * 4B = 1 KB of memory per page

};


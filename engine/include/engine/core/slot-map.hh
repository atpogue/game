#pragma once
#include "engine/core/error.hh"
#include "engine/core/handle.hh"
#include "engine/core/types.hh"
#include <concepts>
#include <vector>

// Use assertions such that unexpected/invalid use fails loudly in debug mode,
// but comes at zero-cost and assumes full trust in release mode.

template <typename Type>
struct SlotMap {

    using size_type       = u32;
    using value_type      = Type;
    using reference       = Type &;
    using const_reference = const Type &;
    using pointer         = Type *;
    using const_pointer   = const Type *;

    explicit SlotMap(u32 limit = UINT32_MAX)
        : slots_()
        , limit_{limit}
        , size_{0u}
        , first_free_{nil}
    {
        PRECONDITION(limit > 0u, "constructed unusable slot map");
    }

    SlotMap(SlotMap &&) noexcept = default;
    SlotMap &operator=(SlotMap &&) noexcept = default;
    SlotMap &operator=(const SlotMap &) = delete;
    ~SlotMap() noexcept = default;

    // Replace the element in the slot and increment the generation, invalidating the old handle.
    // Can be used to represent a new version of the same element, depending on how you interpret generation.
    // If you want to replace the element without changing the generation, use [SlotMap::get] instead.
    // Assumptions: handle is not null, handle has element.
    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Handle<Type> replace(Handle<Type> handle, Args &&... args)
    noexcept(std::is_nothrow_constructible_v<Type, Args...>)
    {
        PRECONDITION(handle, "handle must not be null");
        u32 i = locate(handle);
        PRECONDITION(i != nil, "handle must be live");
        Slot &slot = slots_[i];
        INVARIANT(slot.live, "live handle to a dead slot");
        // doesn't require move constructible
        slot.value.~Type();
        new (&slot.value) Type(std::forward<Args>(args)...);
        ++slot.generation;
        return {i, slot.generation};
    }

    // Fills a slot and returns its handle.
    // Returns null handle if slot limit is reached.
    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Handle<Type> emplace(Args &&... args) {
        u32 i = pop_free_slot();
        if (i == nil) return Handle<Type>::null();
        Slot &slot = slots_[i];
        new (&slot.value) Type(std::forward<Args>(args)...);
        slot.live = true;
        ++size_;
        return {i, slot.generation};
    }

    // Empty a slot and mark it for re-use, destroying the element if it exists.
    // Assumptions: handle is not null, handle has element.
    void erase(Handle<Type> handle) noexcept {
        PRECONDITION(handle, "handle must not be null");
        auto i = locate(handle);
        PRECONDITION(i != nil, "handle must be live");
        Slot &slot = slots_[i];
        INVARIANT(slot.live, "live handle to a dead slot");
        ++slot.generation;
        slot.value.~Type();
        slot.live = false;
        --size_;
        // if generations max out regularly, the slot array will have increasing amounts of unused slots
        // alternatively, generation could be allowed to overflow and wrap around back to 0
        if (slot.generation < limit_) {
            // mark the slot for re-use
            slot.next_free = first_free_;
            first_free_ = i;
        } else slot.next_free = nil;
    }

    void clear() noexcept {
        slots_.clear(); first_free_ = nil; size_ = 0u;
    }

    // Get the full handle associated with the slot at the index.
    [[nodiscard]] Handle<Type> find(u32 index) const noexcept {
        // rename to get_handle?
        return index != nil 
            && index < slots_.size()
            && slots_[index].live == true
            ? Handle<Type>{index, slots_[index].generation} : Handle<Type>::null();
    }

    // Is this handle associated with a value.
    [[nodiscard]] bool has(Handle<Type> handle) const noexcept { return locate(handle) != nil; }

    [[nodiscard]] const_pointer get(Handle<Type> handle) const noexcept {
        auto i = locate(handle);
        return i != nil ? &slots_[i].value : nullptr;
    }

    [[nodiscard]] pointer get(Handle<Type> handle) noexcept {
        auto i = locate(handle);
        return i != nil ? &slots_[i].value : nullptr;
    }

    // Get the number of filled slots.
    [[nodiscard]] u32 size() const noexcept { return size_; }

    [[nodiscard]] u32 limit() const noexcept { return limit_; }

    [[nodiscard]] size_t capacity() const noexcept { return slots_.capacity(); }

    void reserve(u32 n) { slots_.reserve(n); }

    // Explicit copy to prevent unintended implicit copy construction.
    [[nodiscard]] SlotMap copy() const requires std::is_nothrow_copy_constructible_v<Type> { return *this; }

private:

    template <bool IsConst>
    struct Iterator { ///////////////////////////////////////////////////////////////////
    private:

        using ReferenceType = std::conditional_t<IsConst, const Type &, Type &>;
        using OwnerPtr      = std::conditional_t<IsConst, const SlotMap *, SlotMap *>;

    public:

        using iterator_concept  = std::forward_iterator_tag;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::pair<Handle<Type>, ReferenceType>;
        using reference         = value_type;
        using pointer           = void; // proxy; no operator-> provided

        Iterator() : owner_{nullptr}, index_{0u} {}

        Iterator(const Iterator &other) = default;
        Iterator &operator=(const Iterator &other) = default;

        operator Iterator<true>() const noexcept requires (!IsConst) {
            return Iterator<true>(owner_, index_);
        }

        reference operator*() const noexcept {
            PRECONDITION(owner_, "dereferenced singular iterator");
            PRECONDITION(index_ < owner_->slots_.size(), "dereferenced end iterator");
            INVARIANT(owner_->slots_[index_].live == true, "dereferenced dead slot");
            return {
                {index_, owner_->slots_[index_].generation},
                owner_->slots_[index_].value
            };
        }

        Iterator &operator++() noexcept {
            PRECONDITION(owner_, "incremented singular iterator");
            do {
                ++index_;
            } while (index_ < owner_->slots_.size() && !owner_->slots_[index_].live);
            return *this;
        }

        Iterator operator++(int) noexcept { auto temp = *this; ++(*this); return temp; }

        bool operator==(const Iterator &) const noexcept = default;

    private:

        friend struct SlotMap<Type>;

        Iterator(OwnerPtr map, u32 index) noexcept
            : owner_{map}, index_{index}
        {
            INVARIANT(owner_, "constructed without parent container");
            INVARIANT(index_ <= owner_->slots_.size(), "constructed with invalid index");
        }

        OwnerPtr owner_;
        u32 index_;

    }; //////////////////////////////////////////////////////////////////////////////////

public:

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    [[nodiscard]] const_iterator begin() const noexcept {
        u32 i = 0u;
        while (i < slots_.size() && !slots_[i].live) i++;
        return const_iterator(this, i);
    }

    [[nodiscard]] iterator begin() noexcept {
        u32 i = 0u;
        while (i < slots_.size() && !slots_[i].live) i++;
        return iterator(this, i);
    }

    [[nodiscard]] const_iterator end() const noexcept { return const_iterator(this, slots_.size()); }
    [[nodiscard]] iterator       end()       noexcept { return iterator(this, slots_.size()); }

private:

    SlotMap(const SlotMap &) requires std::is_copy_constructible_v<Type> = default;

    struct Slot { ///////////////////////////////////////////////////////////////////////

        Slot() noexcept : live(false), generation(0u), next_free(nil) {}

        Slot(const Slot &other) noexcept(std::is_nothrow_copy_constructible_v<Type>)
            : live(other.live), generation(other.generation)
        {
            if (live) {
                new (&value) Type(other.value);
            } else next_free = other.next_free;
        }

        Slot &operator=(const Slot &other) noexcept(std::is_nothrow_copy_constructible_v<Type>) {
            PRECONDITION(&other != this);
            live = other.live;
            generation = other.generation;
            if (live) {
                new (&value) Type(other.value);
            } else next_free = other.next_free;
            return *this;
        }

        Slot(Slot &&other) noexcept(std::is_nothrow_move_constructible_v<Type>)
            : live(other.live), generation(other.generation)
        {
            if (live) {
                new (&value) Type(std::move(other.value));
                other.live = false;
            } else next_free = other.next_free;
        }

        Slot &operator=(Slot &&other) noexcept(std::is_nothrow_move_constructible_v<Type>) {
            PRECONDITION(&other != this);
            live = other.live;
            generation = other.generation;
            if (live) {
                new (&value) Type(std::move(other.value));
                other.live = false;
            } else next_free = other.next_free;
            return *this;
        }

        ~Slot() noexcept { if (live) value.~Type(); }

        bool live;
        u32 generation;

        // use placement new to set value
        // never read/write next_free if the slot is live
        // never read/write value if the slot is dead
        // always call value destructor before changing status
        union {
            u32 next_free;
            Type value;
        };

    }; //////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] u32 locate(Handle<Type> handle) const noexcept {
        return handle.index != nil 
            && handle.index < slots_.size()
            && slots_[handle.index].live
            && handle.generation != nil
            && handle.generation == slots_[handle.index].generation
            ? handle.index : nil;
    }

    // remove slot from the free list or create a slot if free list is empty
    [[nodiscard]] u32 pop_free_slot() {
        u32 i = first_free_;
        if (i == nil) {
            // append a new slot
            i = slots_.size();
            if (i >= limit_) return nil;
            slots_.emplace_back();
        } else {
            // re-use the slot
            INVARIANT(i < slots_.size(), "invalid index in free list");
            INVARIANT(!slots_[i].live, "live slot in free list");
            first_free_ = slots_[i].next_free;
        }
        return i;
    }

    std::vector<Slot> slots_;
    u32 limit_; // should not change after construction except for move assignment
    u32 size_;
    u32 first_free_;

};


#pragma once
#include "engine/core/handle.hh"
#include "engine/core/types.hh"
#include <cassert>
#include <concepts>
#include <vector>

// Use assertions such that unexpected/invalid use fails loudly in debug mode,
// but comes at zero-cost and assumes full trust in release mode.

template <typename Type>
struct SlotMap {

    SlotMap(u32 limit = UINT32_MAX)
        : slots_()
        , limit_{limit}
        , size_{0u}
        , first_free_{nil}
    {
        assert(limit > 0u && "constructed unusable slot map");
    }

    SlotMap(SlotMap &&) = default;
    SlotMap &operator=(SlotMap &&) = default;

    SlotMap &operator=(const SlotMap &) = delete;

    // Replace the element in the slot and increment the generation, invalidating the old handle.
    // Can be used to represent a new version of the same element, depending on how you interpret generation.
    // If you want to replace the element without changing the generation, use [SlotMap::get] instead.
    // Assumptions: handle is not null, handle has element.
    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Handle<Type> replace(Handle<Type> handle, Args &&... args) {
        assert(handle && "replace on null handle");
        u32 i = locate(handle);
        assert(i != nil && "replace on dead handle");
        Slot &slot = slots_[i];
        assert(slot.live);
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
    void erase(Handle<Type> handle) {
        assert(handle && "erase on null handle");
        auto i = locate(handle);
        assert(i != nil && "erase on dead handle");
        Slot &slot = slots_[i];
        assert(slot.live);
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

    void clear() { slots_.clear(); first_free_ = 0u; size_ = 0u; }

    // Get the full handle associated with the slot at the index.
    [[nodiscard]] constexpr Handle<Type> find(u32 index) const {
        // rename to get_handle?
        return index != nil 
            && index < slots_.size()
            && slots_[index].live == true
            ? Handle<Type>{index, slots_[index].generation} : Handle<Type>::null();
    }

    // Is this handle associated with a value.
    [[nodiscard]] constexpr bool has(Handle<Type> handle) const { return locate(handle) != nil; }

    [[nodiscard]] constexpr const Type *get(Handle<Type> handle) const {
        auto i = locate(handle);
        return i != nil ? &slots_[i].value : nullptr;
    }

    [[nodiscard]] constexpr Type *get(Handle<Type> handle) {
        auto i = locate(handle);
        return i != nil ? &slots_[i].value : nullptr;
    }

    // Get the number of filled slots.
    [[nodiscard]] constexpr u32 size() const { return size_; }

    [[nodiscard]] constexpr u32 limit() const { return limit_; }

    [[nodiscard]] constexpr size_t capacity() const { return slots_.capacity(); }

    constexpr void reserve(u32 n) { slots_.reserve(n); }

    // Explicit copy to prevent unintended implicit copy construction.
    [[nodiscard]] SlotMap copy() const { return *this; }

private:

    template <typename ReferenceType>
    struct Iterator { ///////////////////////////////////////////////////////////////////
        
        using iterator_concept  = std::forward_iterator_tag;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::pair<Handle<Type>, Type>;
        using reference         = std::pair<Handle<Type>, ReferenceType>;
        using pointer           = void;

        Iterator() : owner_{nullptr}, index_{0u} {}

        Iterator(const Iterator &other) = default;
        Iterator &operator=(const Iterator &other) = default;

        reference operator*() const {
            assert(owner_ && "dereferenced singular iterator");
            assert(index_ < owner_->slots_.size() && "dereferenced end iterator");
            assert(owner_->slots_[index_].live == true && "dereferenced dead slot");
            return {
                {index_, owner_->slots_[index_].generation},
                owner_->slots_[index_].value
            };
        }

        Iterator &operator++() {
            assert(owner_ && "incremented singular iterator");
            if (index_ >= owner_->slots_.size()) return *this;
            do { ++index_; }
            while (index_ < owner_->slots_.size() && !owner_->slots_[index_].live);
            return *this;
        }

        Iterator operator++(int) { auto temp = *this; ++(*this); return temp; }

        bool operator==(const Iterator &) const = default;

    private:

        friend struct SlotMap<Type>;

        Iterator(const SlotMap *map, u32 index)
            : owner_{map}, index_{index}
        {
            assert(owner_ && "constructed without parent container");
            assert(index_ <= owner_->slots_.size() && "constructed with invalid index");
        }

        const SlotMap *owner_;
        u32 index_;

    }; //////////////////////////////////////////////////////////////////////////////////

public:

    using iterator = Iterator<Type &>;
    using const_iterator = Iterator<const Type &>;

    [[nodiscard]] constexpr const_iterator begin() const {
        u32 i = 0u;
        while (i < slots_.size() && !slots_[i].live) i++;
        return const_iterator(this, i);
    }

    [[nodiscard]] constexpr iterator begin() {
        u32 i = 0u;
        while (i < slots_.size() && !slots_[i].live) i++;
        return iterator(this, i);
    }

    [[nodiscard]] constexpr const_iterator end() const { return const_iterator(this, size()); }
    [[nodiscard]] constexpr iterator       end()       { return iterator(this, size()); }

private:

    SlotMap(const SlotMap &) = default;

    struct Slot { ///////////////////////////////////////////////////////////////////////

        Slot() noexcept : live(false), generation(0u), next_free(nil) {}

        Slot(const Slot &other) 
            : live(other.live), generation(other.generation)
        {
            if (live) {
                new (&value) Type(other.value);
            } else next_free = other.next_free;
        }

        Slot& operator=(const Slot &other) {
            live = other.live;
            generation = other.generation;
            if (live) {
                new (&value) Type(other.value);
            } else next_free = other.next_free;
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
            live = other.live;
            generation = other.generation;
            if (live) {
                new (&value) Type(std::move(other.value));
                other.live = false;
            } else next_free = other.next_free;
        }

        ~Slot() { if (live) value.~Type(); }

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

    [[nodiscard]] constexpr u32 locate(Handle<Type> handle) const {
        return handle.index != nil 
            && handle.index < slots_.size()
            && slots_[handle.index].live
            && handle.generation != nil
            && handle.generation == slots_[handle.index].generation
            ? handle.index : nil;
    }

    // remove slot from the free list
    [[nodiscard]] u32 pop_free_slot() {
        u32 i = first_free_;
        if (i == nil) {
            // append a new slot
            i = slots_.size();
            if (i >= limit_) return nil;
            slots_.emplace_back();
        } else {
            // re-use the slot
            first_free_ = slots_[i].next_free;
        }
        assert(!slots_[i].live);
        return i;
    }

    std::vector<Slot> slots_;
    u32 limit_; // should not change after construction except for move assignment
    u32 size_;
    u32 first_free_;

};


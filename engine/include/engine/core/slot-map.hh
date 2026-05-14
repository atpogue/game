#pragma once
#include "engine/core/handle.hh"
#include "engine/core/types.hh"
#include <cassert>
#include <concepts>
#include <vector>

template <typename Type>
struct SlotMap {

    SlotMap() = default;

    SlotMap(SlotMap &&) = default;
    SlotMap &operator=(SlotMap &&) = default;

    SlotMap &operator=(const SlotMap &) = delete;

    struct Item { const Handle<Type> handle; Type &value; };
    struct ConstItem { const Handle<Type> handle; const Type &value; };

    // Replace the element in the slot and increment the generation, invalidating the old handle.
    // Can be used to represent a new version of the same element, depending on how you interpret generation.
    // If you want to replace the element without changing the generation, use [SlotMap::get] instead.
    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Handle<Type> replace(Handle<Type> handle, Args &&... args) {
        u32 i = locate(handle);
        if (i == nil) return nullptr;
        Slot &slot = slots_[i];
        assert(slot.live);
        slot.value.~Type();
        new (&slot.value) Type(std::forward<Args>(args)...);
        ++slot.generation;
        return {i, slot.generation};
    }

    // Fills a slot and returns its handle.
    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Handle<Type> emplace(Args &&... args) {
        u32 i = pop_free_slot();
        if (i == nil) return Handle<Type>::null();
        return fill_slot(i, std::forward<Args>(args)...);
    }

    // Empty a slot and mark it for re-use, destroying the element if it exists.
    void erase(Handle<Type> handle) {
        auto i = locate(handle);
        if (i == nil) return;
        Slot &slot = slots_[i];
        assert(slot.live);
        ++slot.generation;
        slot.value.~Type();
        slot.live = false;
        --size_;
        // if generations max out regularly, the slot array will have increasing amounts of unused slots
        // alternatively, generation could be allowed to overflow and wrap around back to 0
        if (slot.generation < index_max) {
            // mark the slot for re-use
            slot.next_free = first_free_;
            first_free_ = i;
        } else slot.next_free = nil;
    }

    void clear() { slots_.clear(); }

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

    [[nodiscard]] constexpr size_t capacity() const { return slots_.capacity(); }

    constexpr void reserve(u32 n) { slots_.reserve(n); }

    // Explicit copy to prevent unintended implicit copy construction.
    [[nodiscard]] SlotMap copy() const { return *this; }

private:

    SlotMap(const SlotMap &) = default;

    template <typename Value>
    struct Iterator { ///////////////////////////////////////////////////////////////////
        using iterator_concept  = std::forward_iterator_tag;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using reference         = Value;

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

    std::vector<Slot> slots_;
    u32 first_free_ = nil;
    u32 size_ = 0u;

protected:

    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Handle<Type> fill_slot(u32 i, Args &&... args) {
        assert(i != nil && i < slots_.size());
        Slot &slot = slots_[i];
        new (&slot.value) Type(std::forward<Args>(args)...);
        slot.live = true;
        ++size_;
        return {i, slot.generation};
    }

    constexpr void next_generation(u32 i) noexcept {
        assert(i != nil && i < slots_.size());
        ++slots_[i].generation;
    }

    // remove slot from the free list
    [[nodiscard]] u32 pop_free_slot() {
        u32 i = first_free_;
        if (i == nil) {
            // append a new slot
            i = slots_.size();
            if (i >= index_max) return nil;
            slots_.emplace_back();
        } else {
            // re-use the slot
            first_free_ = slots_[i].next_free;
        }
        assert(!slots_[i].live);
        return i;
    }

    // add slot to the free list
    constexpr void push_free_slot(u32 i) noexcept {
        assert(i != nil && i < slots_.size());
        assert(!slots_[i].live);
        slots_[i].next_free = first_free_;
        first_free_ = i;
    }

public:

    using iterator = Iterator<Item>;
    using const_iterator = Iterator<ConstItem>;

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

};


#pragma once
#include "engine/core/handle.hh"
#include "engine/core/types.hh"
#include <cassert>
#include <concepts>
#include <vector>

template <typename Type>
struct SlotMap {

    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Type *replace(Handle<Type> handle, Args &&... args) {
        u32 i = locate(handle);
        if (i == handle_index_max) return nullptr;
        Slot &slot = slots[i];
        assert(slot.live);
        slot.value.~Type();
        new (&slot.value) Type(std::forward<Args>(args)...);
        return &slot.value;
    }

    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Handle<Type> emplace(Args &&... args) {
        if (first_free == handle_index_max) {
            if (slots.size() >= handle_index_max) return Handle<Type>::null();
            Slot &slot = slots.emplace_back();
            new (&slot.value) Type(std::forward<Args>(args)...);
            slot.live = true;
            return {size()-1u, 0u};
        }
        auto i = first_free;
        Slot &slot = slots[i];
        assert(!slot.live);
        first_free = slot.next_free;
        new (&slot.value) Type(std::forward<Args>(args)...);
        slot.live = true;
        return {i, slot.generation};
    }

    void erase(Handle<Type> &handle) {
        auto i = locate(handle);
        if (i == handle_index_max) return;
        Slot &slot = slots[i];
        slot.live = false;
        slot.generation++;
        slot.value.~Type();
        if (slot.generation >= handle_generation_max) {
            // retire the slot
            // this means handles are guaranteed to be unique and stable
            // but also means if generations max out regularly,
            // the slot array will have increasing amounts of unused slots
            slot.next_free = handle_index_max;
            return;
        }
        // mark the slot for re-use
        slot.next_free = first_free;
        first_free = i;
	}

    void clear() { slots.clear(); }

    constexpr u32 generation(u32 index) const {
        return index < handle_index_max 
            && index < slots.size()
            && slots[index].live
            ? slots[index].generation : handle_generation_max;
    }

    constexpr bool is_alive(Handle<Type> handle) const { return locate(handle) != handle_index_max; }

    constexpr const Type *get(Handle<Type> handle) const {
        auto i = locate(handle);
        return i != handle_index_max ? &slots[i].value : nullptr;
    }

    constexpr Type *get(Handle<Type> handle) {
        auto i = locate(handle);
        return i != handle_index_max ? &slots[i].value : nullptr;
    }

    constexpr u32 size() const { return static_cast<u32>(slots.size()); };

    constexpr size_t capacity() const { return slots.capacity(); }

    constexpr void reserve(u32 n) { slots.reserve(n); }

private:

    template <typename Value>
    struct Iterator { ///////////////////////////////////////////////////////////////////
        using iterator_concept  = std::forward_iterator_tag;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using reference         = Value;

        Iterator() : map{nullptr}, index{0u} {}

        Iterator(const Iterator &other) = default;
        Iterator &operator=(const Iterator &other) = default;

        reference operator*() const {
            assert(map && "dereferenced singular iterator");
            assert(index < map->slots.size() && "dereferenced end iterator");
            assert(map->slots[index].live && "dereferenced dead slot");
            return {
                {index, map->slots[index].generation},
                map->slots[index].value
            };
        }

        Iterator &operator++() {
            assert(map && "incremented singular iterator");
            if (index >= map->slots.size()) return *this;
            do { ++index; } while (index < map->slots.size() && !map->slots[index].live);
            return *this;
        }

        Iterator operator++(int) { auto temp = *this; ++(*this); return temp; }

        bool operator==(const Iterator &) const = default;

    private:

        friend struct SlotMap<Type>;

        Iterator(const SlotMap *map, u32 index)
            : map{map}, index{index}
        {
            assert(index <= map->slots.size() && "constructed with invalid index");
        }

        const SlotMap *map;
        u32 index;

    }; //////////////////////////////////////////////////////////////////////////////////

    struct Slot { ///////////////////////////////////////////////////////////////////////

        Slot() noexcept : live(false), generation(0u), next_free(handle_index_max) {}

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
        // always call value destructor before changing is_alive
        union {
            u32 next_free;
            Type value;
        };

    }; //////////////////////////////////////////////////////////////////////////////////

    constexpr u32 locate(Handle<Type> handle) const {
        return handle.index < handle_index_max 
            && handle.index < slots.size()
            && slots[handle.index].live
            && handle.generation < handle_generation_max
            && handle.generation == slots[handle.index].generation
            ? handle.index : handle_index_max;
    }

    std::vector<Slot> slots;
    u32 first_free = handle_index_max;

public:

    struct Item { const Handle<Type> handle; Type &value; };
    struct ConstItem { const Handle<Type> handle; const Type &value; };

    using iterator = Iterator<Item>;
    using const_iterator = Iterator<ConstItem>;

    constexpr const const_iterator begin() const {
        u32 i = 0u;
        while (i < slots.size() && !slots[i].live) i++;
        return const_iterator(this, i);
    }

    constexpr iterator begin() {
        u32 i = 0u;
        while (i < slots.size() && !slots[i].live) i++;
        return iterator(this, i);
    }

    constexpr const const_iterator end() const { return const_iterator(this, size()); }
    constexpr       iterator       end()       { return iterator(this, size()); }

};


template <>
struct SlotMap<void> {

    Handle<void> emplace() {
        if (first_free == handle_index_max) {
            if (slots.size() >= handle_index_max) return Handle<void>::null();
            Slot &slot = slots.emplace_back();
            slot.live = true;
            return {size()-1u, 0u};
        }
        auto i = first_free;
        Slot &slot = slots[i];
        assert(!slot.live);
        first_free = slot.next_free;
        slot.live = true;
        return {i, slot.generation};
    }

    void erase(Handle<void> &handle) {
        auto i = locate(handle);
        if (i == handle_index_max) return;
        Slot &slot = slots[i];
        slot.live = false;
        slot.generation++;
        if (slot.generation >= handle_generation_max) {
            // retire the slot
            // this means handles are guaranteed to be unique and stable
            // but also means if generations max out regularly,
            // the slot array will have increasing amounts of unused slots
            slot.next_free = handle_index_max;
            return;
        }
        // mark the slot for re-use
        slot.next_free = first_free;
        first_free = i;
	}

    void clear() { slots.clear(); }

    constexpr u32 generation(u32 index) const {
        return index < handle_index_max 
            && index < slots.size()
            && slots[index].live
            ? slots[index].generation : handle_generation_max;
    }

    constexpr bool is_alive(Handle<void> handle) const { return locate(handle) != handle_index_max; }

    constexpr u32 size() const { return static_cast<u32>(slots.size()); };

    constexpr size_t capacity() const { return slots.capacity(); }

    constexpr void reserve(u32 n) { slots.reserve(n); }

    struct Iterator { ///////////////////////////////////////////////////////////////////
        // Is this code duplication avoidable?
        using iterator_concept  = std::forward_iterator_tag;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Handle<void>;

        Iterator() : map{nullptr}, index{0u} {}

        Iterator(const Iterator &other) = default;
        Iterator &operator=(const Iterator &other) = default;

        value_type operator*() const {
            assert(map && "dereferenced singular iterator");
            assert(index < map->slots.size() && "dereferenced end iterator");
            assert(map->slots[index].live && "dereferenced dead slot");
            return {index, map->slots[index].generation};
        }

        Iterator &operator++() {
            assert(map && "incremented singular iterator");
            if (index >= map->slots.size()) return *this;
            do { ++index; } while (index < map->slots.size() && !map->slots[index].live);
            return *this;
        }

        Iterator operator++(int) { auto temp = *this; ++(*this); return temp; }

        bool operator==(const Iterator &) const = default;

    private:

        friend struct SlotMap<void>;

        Iterator(const SlotMap<void> *map, u32 index)
            : map{map}, index{index}
        {
            assert(index <= map->slots.size() && "constructed with invalid index");
        }

        const SlotMap *map;
        u32 index;

    }; //////////////////////////////////////////////////////////////////////////////////

    constexpr const Iterator begin() const {
        u32 i = 0u;
        while (i < slots.size() && !slots[i].live) i++;
        return Iterator(this, i);
    }

    constexpr const Iterator end() const { return Iterator(this, size()); }

private:

    struct Slot {
        bool live = false;
        u32 generation = 0u;
        u32 next_free = handle_index_max;
    };

    constexpr u32 locate(Handle<void> handle) const {
        return handle.index < handle_index_max 
            && handle.index < slots.size()
            && slots[handle.index].live
            && handle.generation < handle_generation_max
            && handle.generation == slots[handle.index].generation
            ? handle.index : handle_index_max;
    }

    std::vector<Slot> slots;
    u32 first_free = handle_index_max;

};


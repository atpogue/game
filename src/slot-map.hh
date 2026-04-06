#pragma once
#include "handle.hh"
#include <cassert>
#include <concepts>
#include <vector>

template <typename Type>
struct SlotMap {

    ~SlotMap() {
        for (Slot &slot : slots) {
            if (slot.live) slot.value.~Type();
        }
    }

    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Type &replace(Handle<Type> handle, Args &&... args) {
        uint32_t i = locate(handle);
        if (i == handle_index_max) return;
        Slot &slot = slots[i];
        assert(slot.live);
        slot.value.~Type();
        new (&slot.value) Type(std::forward<Args>(args)...);
        return slot.value;
    }

    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    Handle<Type> emplace(Args &&... args) {
        if (first_free == handle_index_max) {
            if (slots.size() >= handle_index_max) return Handle<Type>::null();
            Slot &slot = slots.emplace_back(true, 0u);
            new (&slot.value) Type(std::forward<Args>(args)...);
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

    constexpr uint32_t generation(uint32_t index) const {
        return index < handle_index_max 
            && index < slots.size()
            && slots[index].live
            ? slots[index].generation : handle_generation_max;
    }

    constexpr bool status(Handle<Type> handle) const { return locate(handle) != handle_index_max; }

    constexpr const Type *get(Handle<Type> handle) const {
        auto i = locate(handle);
        return i != handle_index_max ? &slots[i].value : nullptr;
    }

    constexpr Type *get(Handle<Type> handle) {
        auto i = locate(handle);
        return i != handle_index_max ? &slots[i].value : nullptr;
    }

    constexpr uint32_t size() const { return static_cast<uint32_t>(slots.size()); };

    constexpr size_t capacity() const { return slots.capacity(); }

    constexpr void reserve(uint32_t n) { slots.reserve(n); }

    struct Item { const Handle<Type> handle; Type &type; };
    struct ConstItem { const Handle<Type> handle; const Type &type; };

    struct ConstIterator { //////////////////////////////////////////////////////////////

        ConstIterator(const ConstIterator &other) = default;
        ConstIterator &operator=(const ConstIterator &other) = default;

        ConstItem operator*() {
            assert(index < map->slots.size() && "dereferenced end iterator");
            assert(map->slots[index].live && "dereferenced dead slot");
            return {
                {index, map->slots[index].generation},
                map->slots[index].value
            };
        }

        ConstIterator &operator++() {
            if (index >= map->slots.size()) return *this;
            do { ++index; } while (index < map->slots.size() && !map->slots[index].live);
            return *this;
        }

        ConstIterator operator++(int) { auto temp = *this; ++(*this); return temp; }

        bool operator==(const ConstIterator &) const = default;

    private:

        friend struct SlotMap;

        ConstIterator(const SlotMap *map, uint32_t index)
            : map{map}, index{index}
        {
            assert(index <= map->slots.size() && "constructed with invalid index");
        }

        const SlotMap *map;
        uint32_t index;

    }; //////////////////////////////////////////////////////////////////////////////////

    struct Iterator { ///////////////////////////////////////////////////////////////////
        // Is this code duplication avoidable?

        Iterator(const Iterator &other) = default;
        Iterator &operator=(const Iterator &other) = default;

        Item operator*() {
            assert(index < map->slots.size() && "dereferenced end iterator");
            assert(map->slots[index].live && "dereferenced dead slot");
            return {
                {index, map->slots[index].generation},
                map->slots[index].value
            };
        }

        Iterator &operator++() {
            if (index >= map->slots.size()) return *this;
            do { ++index; } while (index < map->slots.size() && !map->slots[index].live);
            return *this;
        }

        Iterator operator++(int) { auto temp = *this; ++(*this); return temp; }

        bool operator==(const Iterator &) const = default;

    private:

        friend class SlotMap;

        Iterator(SlotMap *map, uint32_t index)
            : map{map}, index{index}
        {
            assert(index <= map->slots.size() && "constructed with invalid index");
        }

        SlotMap *map;
        uint32_t index;

    }; //////////////////////////////////////////////////////////////////////////////////

    constexpr const ConstIterator begin() const {
        uint32_t i = 0u;
        while (i > slots.size() && !slots[i].live) i++;
        return ConstIterator(this, i);
    }

    constexpr Iterator begin() {
        uint32_t i = 0u;
        while (i > slots.size() && !slots[i].live) i++;
        return Iterator(this, i);
    }

    constexpr const ConstIterator end() const { return Iterator(this, size()); }
    constexpr       Iterator      end()       { return Iterator(this, size()); }

private:

    friend struct ConstIterator;
    friend struct Iterator;

    struct Slot {
        bool live = false;
        uint32_t generation = 0u;
        // use placement new to set value
        // never read/write next_free if the slot is live
        // never read/write value if the slot is dead
        // always call value destructor before changing status
        union {
            uint32_t next_free = handle_index_max;
            Type value;
        };
    };

    constexpr uint32_t locate(Handle<Type> handle) const {
        return handle.index < handle_index_max 
            && handle.index < slots.size()
            && slots[handle.index].live
            && handle.generation < handle_generation_max
            && handle.generation == slots[handle.index].generation
            ? handle.index : handle_index_max;
    }

    std::vector<Slot> slots;
    uint32_t first_free = handle_index_max;

};


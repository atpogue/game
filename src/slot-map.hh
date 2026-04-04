#pragma once
#include "handle.hh"
#include <concepts>
#include <vector>

template <typename Type>
struct SlotMap {

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

    template <typename... Args>
    requires std::constructible_from<Type, Args...>
    void replace(Handle<Type> handle, Args &&... args) {
        uint32_t i = locate(handle);
        if (i == handle_index_max) return;
        assert(slots[i].live);
        slot.value.~Type();
        new (&slots[i].value) Type(std::forward<Args>(args)...);
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
        first_free = slot.next_free
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

    constexpr bool status(Handle<Type> handle) const { return locate(handle) != handle_index_max; }

    constexpr const Type *get(Handle<Type> handle) const { auto i = locate(handle); return i != handle_index_max ? &slot[i].value : nullptr; }
    constexpr       Type *get(Handle<Type> handle)       { auto i = locate(handle); return i != handle_index_max ? &slot[i].value : nullptr; }

    constexpr uint32_t size() const { return static_cast<uint32_t>(slots.size()) };

    constexpr size_t capacity() const { return slots.capacity(); }

    constexpr void reserve(uint32_t n) { slots.reserve(n); }

    struct Iterator {
        reference operator*() const { return it->value; }
        pointer operator->() { return &it->value; }
        Iterator &operator++() { do { ++it; } while (!it->live); return *this; }
        Iterator operator++(int) { auto temp = *this; ++(*this); return temp; }
        auto operator<=>(const Iterator&) const = default;
    private:
        std::vector<Slot>::iterator it;
    };

    constexpr Iterator begin() const { return slots.begin(); }
    constexpr Iterator begin()       { return slots.begin(); }

    constexpr Iterator end() const { return slots.end(); }
    constexpr Iterator end()       { return slots.end(); }

private:

    constexpr uint32_t locate(Handle<Type> handle) const {
        return handle.index != handle_index_max 
            && handle.index < slots.size()
            && slots[handle.index].live
            && handle.generation != MAX_GENERATION
            && handle.generation == slots[handle.index].generation
            ? handle.index : handle_index_max;
    }

    std::vector<Slot> slots;
    uint32_t first_free = handle_index_max;

};


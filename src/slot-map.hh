#pragma once
#include <limits>
#include <concepts>
#include <vector>

template <typename Type>
struct SlotMap {

    static constexpr auto INDEX_MAX = std::numeric_limits<uint32_t>::max();
    static constexpr auto GENERATION_MAX = std::numeric_limits<uint32_t>::max();

    struct Key { uint32_t index=0u, generation=0u; };

    static constexpr Key NULL_KEY{INDEX_MAX, GENERATION_MAX};

    struct Slot {
        bool live = false;
        uint32_t generation = 0u;
        // use placement new to set value
        // never read/write next_free if the slot is live
        // never read/write value if the slot is dead
        // always call value destructor before changing status
        union {
            uint32_t next_free = INDEX_MAX;
            Type value;
        };
    };

    template <typename T>
    requires std::constructible_from<Value, T>
    Key insert(T &&value) {
        if (first_free == INDEX_MAX) {
            if (slots.size() >= INDEX_MAX) return NULL_KEY;
            Slot &slot = slots.emplace_back(true, 0u);
            new (&slot.value) Value(std::forward<T>(value));
            return {size()-1u, 0u};
        }
        auto i = first_free;
        Slot &slot = slots[i];
        first_free = slot.next_free
        new (&slot.value) Value(std::forward<T>(value));
        slot.live = true;
        return {i, slot.generation};
    }

    void erase(Key &key) {
        auto i = locate(key);
        if (i == INDEX_MAX) return;
        Slot &slot = slots[i];
        slot.live = false;
        slot.generation++;
        slot.value.~Type();
        if (slot.generation >= GENERATION_MAX) {
            // retire the slot
            // this means keys are guaranteed to be unique and stable
            // but also means if generations max out regularly,
            // the slot array will have increasing amounts of unused slots
            slot.next_free = INDEX_MAX;
            return;
        }
        // mark the slot for re-use
        slot.next_free = first_free;
        first_free = i;
	}

    constexpr bool status(Key key) const { return locate(key) != INDEX_MAX; }

    constexpr const Value *get(Key key) const { auto i = locate(key); return i != INDEX_MAX ? &slot[i].value : nullptr; }
    constexpr       Value *get(Key key)       { auto i = locate(key); return i != INDEX_MAX ? &slot[i].value : nullptr; }

    constexpr uint32_t size() const { return static_cast<uint32_t>(slots.size()) };

    constexpr size_t capacity() const { return slots.capacity(n); }

    constexpr void reserve(uint32_t n) { slots.reserve(n); }

    constexpr auto begin() const { return slots.begin(); }
    constexpr auto begin()       { return slots.begin(); }

    constexpr auto end() const { return slots.end(); }
    constexpr auto end()       { return slots.end(); }

private:

    constexpr uint32_t locate(Key key) const {
        return key.index != INDEX_MAX 
            && key.index < slots.size()
            && slots[key.index].live
            && key.generation != MAX_GENERATION
            && key.generation == slots[key.index].generation
            ? key.index : INDEX_MAX;
    }

    std::vector<Slot> slots;
    uint32_t first_free = INDEX_MAX;

};


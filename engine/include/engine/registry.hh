#pragma once
#include "engine/core/invariant.hh"
#include "engine/core/slot-map.hh"
#include "engine/core/sparse-set.hh"
#include "engine/core/type-info.hh"
#include "engine/entity.hh"
#include <vector>
#include <memory>

// TODO: ability to query all entity with a series of components

// Type-erased interface to component storage.
struct AnyComponentStore {
    virtual ~AnyComponentStore() noexcept = default;
    virtual bool has(u32 index) const = 0;
    virtual void erase(u32 index) = 0;
};

template <typename Type>
struct ComponentStore : AnyComponentStore, SparseSet<Type> {
    bool has(u32 i) const override { return SparseSet<Type>::has(i); }
    void erase(u32 i) override { SparseSet<Type>::erase(i); }
};

// Use assertions to check internal logic, invariants to check external input.

// Assumptions: [TypeInfo::count] is fixed after Registry construction.
template <TypeInfo Info, typename EntityData = Nothing>
struct Registry { ///////////////////////////////////////////////////////////////////////

    Registry(u32 limit = UINT32_MAX)
        : entities_(limit)
        , stores_(Info::count())
    {}

    Registry(Registry &&) = default;
    Registry &operator=(Registry &&) = default;

    Registry &operator=(const Registry &) = delete;

    template <typename T, typename... Args>
    requires std::constructible_from<T, Args...>
    T &emplace(Entity e, Args &&... args) {
        assert(entities_.has(handle(e)) && "emplace on non-existent entity");
        return get_or_create_store<T>().emplace(index(e), std::forward<Args>(args)...);
    }

    template <typename T>
    void erase(Entity e) {
        assert(entities_.has(handle(e)) && "erase on non-existent entity");
        if (auto store = get_store<T>()) store->erase(index(e));
    }

    bool is_alive(Entity e) const { return entities_.has(handle(e)); }

    void destroy(Entity e) {
        assert(entities_.has(handle(e)) && "destroy on non-existent entity");
        const u32 i = index(e); 
        for (auto &store : stores_)
            if (store && store->has(i)) store->erase(i);
        entities_.erase(handle(e));
    }

    template <typename T>
    [[nodiscard]] T *get(Entity e) {
        auto store = get_store<T>();
        return store ? store->get(index(e)) : nullptr;
    }

    template <typename T>
    [[nodiscard]] const T *get(Entity e) const {
        auto store = get_store<T>();
        return store ? store->get(index(e)) : nullptr;
    }

    template <typename T, typename... Ts>
    bool has(Entity e) const {
        return  has(e, Info::template index<T>())
            && (has(e, Info::template index<Ts>()) && ...);
    }

    [[nodiscard]] Entity create() {
        auto handle = entities_.emplace();
        INVARIANT(handle, "Entity limit reached!");
        return (Handle<void>)handle;
    }

    [[nodiscard]] constexpr u32 capacity() const { return entities_.capacity(); }

    // Gets the number of live entities;
    [[nodiscard]] u32 size() const { return entities_.size(); }

    [[nodiscard]] constexpr u32 limit() const { return entities_.limit(); }

    void clear() {
        for (auto &ptr : stores_) ptr.reset();
        entities_.clear();
    }

    struct Iterator { ///////////////////////////////////////////////////////////////////

        using iterator_concept  = std::forward_iterator_tag;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::pair<Entity, EntityData>;
        using reference         = std::pair<Entity, const EntityData &>;
        using pointer           = void;

        Iterator() = default;
        Iterator(const Iterator &other) = default;

        Iterator &operator=(const Iterator &other) = default;

        reference operator*() const { return {(Handle<void>)(*slot_).first, (*slot_).second}; }

        Iterator &operator++() { ++slot_; return *this; }
        Iterator operator++(int) { auto temp = *this; ++(*this); return temp; }

        bool operator==(const Iterator &) const = default;

    private:

        friend struct Registry;

        Iterator(SlotMap<EntityData>::const_iterator slot) : slot_(slot) {}

        SlotMap<EntityData>::const_iterator slot_;

    }; //////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] Iterator begin() const { return Iterator(entities_.begin()); }
    [[nodiscard]] Iterator end()   const { return Iterator(entities_.end()); }

    // Explicit copy to prevent unintended implicit copy construction.
    [[nodiscard]] Registry copy() const { return *this; }

private:

    Registry(const Registry &) = default;

    static constexpr u32 index(Entity e) { return ((Handle<void>)e).index; }

    static constexpr Handle<EntityData> handle(Entity e) {
        auto t = (Handle<void>)e;
        return { t.index, t.generation };
    }

    bool has(Entity e, u32 i) const {
        assert(i < stores_.size());
        return stores_[i] ? stores_[i]->has(index(e)) : false;
    }

    template <typename T>
    [[nodiscard]] constexpr ComponentStore<T> *get_store() {
        assert(Info::template index<T>() < stores_.size());
        auto &store = stores_[Info::template index<T>()];
        return static_cast<ComponentStore<T> *>(store.get());
    }

    template <typename T>
    [[nodiscard]] constexpr ComponentStore<T> *get_store() const {
        assert(Info::template index<T>() < stores_.size());
        auto &store = stores_[Info::template index<T>()];
        return static_cast<ComponentStore<T> *>(store.get());
    }

    template <typename T>
    [[nodiscard]] ComponentStore<T> &get_or_create_store() {
        assert(Info::template index<T>() < stores_.size());
        auto &store = stores_[Info::template index<T>()];
        if (!store) store = std::make_unique<ComponentStore<T>>();
        return *static_cast<ComponentStore<T> *>(store.get());
    }

    SlotMap<EntityData> entities_;
    // vector not array to allow use of a component set that is determined at run-time
    std::vector<std::unique_ptr<AnyComponentStore>> stores_;

}; //////////////////////////////////////////////////////////////////////////////////////


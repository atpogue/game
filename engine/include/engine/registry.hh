#pragma once
#include "engine/core/invariant.hh"
#include "engine/core/slot-map.hh"
#include "engine/core/sparse-map.hh"
#include "engine/entity.hh"
#include "engine/signature.hh"
#include <array>
#include <memory>

// Type-erased interface to component storage.
struct RegistryStore {
    virtual ~RegistryStore() = default;
    virtual bool has(u32 index) const = 0;
    virtual void erase(u32 index) = 0;
};

template <typename T>
struct ComponentStore : RegistryStore, SparseMap<T> {
    bool has(u32 i) const override { return SparseMap<T>::has(i); }
    void erase(u32 i) override { SparseMap<T>::erase(i); }
};

struct Registry { ///////////////////////////////////////////////////////////////////////
    // it is possible to convert this to a template
    // so that the components it supports isn't defined by the engine
    // using Components = TypeList<Ts...>;
    // using Signature = TypeFlag<Ts...>;

    [[nodiscard]] Signature signature(Entity e) const {
        auto sig = entities_.get(handle(e));
        // a dead entity has no components
        return sig ? *sig : Signature();
    }

    template <typename T, typename... Args>
    requires std::constructible_from<T, Args...>
    T &emplace(Entity e, Args &&... args) {
        auto sig = entities_.get(handle(e));
        INVARIANT(sig, "emplace on invalid entity");
        sig->set(Components::template index<T>(), true);
        return get_or_create_store<T>().emplace(index(e), std::forward<Args>(args)...);
    }

    template <typename T>
    void erase(Entity e) {
        auto sig = entities_.get(handle(e));
        if (!sig) return;
        sig->set(Components::template index<T>(), false);
        get_or_create_store<T>().erase(index(e));
    }

    constexpr bool is_alive(Entity e) const { return entities_.has(handle(e)); }

    void destroy(Entity e) {
        auto sig = entities_.get(handle(e));
        if (!sig) return;
        for (u32 c = 0; c < Components::size; ++c) {
            if (sig->has(c)) {
                INVARIANT((bool)stores_[c], "component store not created");
                stores_[c]->erase(index(e));
            }
        }
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

    constexpr bool has(Entity e, Signature match) {
        auto sig = entities_.get(handle(e));
        return sig ? sig->has(match) : match.none();
    }

    template <typename T>
    constexpr bool has(Entity e) const {
        auto sig = entities_.get(handle(e));
        return sig ? sig->has(Components::template index<T>()) : false;
    }

    [[nodiscard]] Entity create() { return (Handle<void>)entities_.emplace(); }

    [[nodiscard]] constexpr u32 capacity() { return entities_.capacity(); }

    // Gets the number of live entities;
    [[nodiscard]] constexpr size_t size() const { return entities_.size(); }

    void clear() { for (auto &ptr : stores_) ptr.reset(nullptr); }

    struct Item { Entity entity; Signature signature; };

    struct Iterator { ///////////////////////////////////////////////////////////////////
        
        using iterator_concept  = std::forward_iterator_tag;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using reference         = Item;

        Iterator() = default;
        Iterator(const Iterator &other) = default;

        Iterator &operator=(const Iterator &other) = default;

        reference operator*() const { return {(Handle<void>)(*slot_).handle, (*slot_).value}; }

        Iterator &operator++() { ++slot_; return *this; }
        Iterator operator++(int) { auto temp = *this; ++(*this); return temp; }

        bool operator==(const Iterator &) const = default;

    private:

        friend struct Registry;

        Iterator(SlotMap<Signature>::const_iterator slot) : slot_(slot) {}

        SlotMap<Signature>::const_iterator slot_;

    }; //////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] constexpr Iterator begin() const { return entities_.begin(); }
    [[nodiscard]] constexpr Iterator end()   const { return entities_.end(); }

private:

    static constexpr u32 index(Entity e) { return ((Handle<void>)e).index; }

    static constexpr Handle<Signature> handle(Entity e) {
        auto t = (Handle<void>)e;
        return { t.index, t.generation };
    }

    template <typename T>
    [[nodiscard]] constexpr ComponentStore<T> *get_store() {
        auto &store = stores_[Components::template index<T>()];
        return static_cast<ComponentStore<T> *>(store.get());
    }

    template <typename T>
    [[nodiscard]] constexpr ComponentStore<T> *get_store() const {
        auto &store = stores_[Components::template index<T>()];
        return static_cast<ComponentStore<T> *>(store.get());
    }

    // constructs the component store if it doesn't already exist
    template <typename T>
    [[nodiscard]] ComponentStore<T> &get_or_create_store() {
        auto &store = stores_[Components::template index<T>()];
        if (!store) store.reset(new ComponentStore<T>());
        return *static_cast<ComponentStore<T> *>(store.get());
    }

    SlotMap<Signature> entities_;
    std::array<std::unique_ptr<RegistryStore>, Components::size> stores_;

}; //////////////////////////////////////////////////////////////////////////////////////


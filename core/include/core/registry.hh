#pragma once
#include "core/entity.hh"
#include "core/slot-map.hh"
#include "core/sparse-set.hh"
#include "core/type-info.hh"
#include <memory>
#include <vector>

// TODO: ability to query all entity with a series of components
// Type-erased interface to component storage.
// Operations that don't require the registry to have the store's type.
struct AnyComponentStore
{
  virtual ~AnyComponentStore() noexcept                           = default;
  virtual bool                               has(u32 index) const = 0;
  virtual void                               erase(u32 index)     = 0;
  virtual std::unique_ptr<AnyComponentStore> clone() const        = 0;
};

template <typename Type>
struct ComponentStore final
  : AnyComponentStore
  , SparseSet<Type>
{
  ComponentStore(SparseSet<Type>&& other) noexcept : SparseSet<Type>(std::move(other)) {}

  ComponentStore()                          = default;
  ComponentStore(ComponentStore&&) noexcept = default;
  ~ComponentStore() noexcept                = default;

  bool has(u32 i) const override { return SparseSet<Type>::has(i); }

  void erase(u32 i) override { SparseSet<Type>::erase(i); }

  std::unique_ptr<AnyComponentStore> clone() const override
  {
    return std::make_unique<ComponentStore<Type>>(SparseSet<Type>::copy());
  }
};

// Assumptions: [TypeInfo::count] is fixed after Registry construction.
template <TypeInfo Info, typename EntityData = Nothing>
struct Registry
{ ///////////////////////////////////////////////////////////////////////

  Registry(u32 limit = UINT32_MAX) : entities_(limit), stores_(Info::count()) {}

  Registry(Registry&&)                 = default;
  Registry& operator=(Registry&&)      = default;
  Registry& operator=(Registry const&) = delete;

  EntityData const* meta(Entity e) const { return entities_.get(handle(e)); }

  EntityData* meta(Entity e) { return entities_.get(handle(e)); }

  template <typename T, typename... Args>
  requires std::constructible_from<T, Args...>
  T& emplace(Entity e, Args&&... args)
  {
    PRECONDITION(entities_.has(handle(e)), "entity must be live");
    return get_or_create_store<T>().emplace(index(e), std::forward<Args>(args)...);
  }

  template <typename T>
  void erase(Entity e)
  {
    PRECONDITION(entities_.has(handle(e)), "entity must be live");
    if (auto store = get_store<T>()) store->erase(index(e));
  }

  bool is_alive(Entity e) const { return entities_.has(handle(e)); }

  void destroy(Entity e)
  {
    PRECONDITION(entities_.has(handle(e)), "entity must be live");
    u32 const i = index(e);
    for (auto& store : stores_)
      if (store && store->has(i)) store->erase(i);
    entities_.erase(handle(e));
  }

  template <typename T>
  [[nodiscard]] T* get(Entity e)
  {
    auto store = get_store<T>();
    return store ? store->get(index(e)) : nullptr;
  }

  template <typename T>
  [[nodiscard]] T const* get(Entity e) const
  {
    auto store = get_store<T>();
    return store ? store->get(index(e)) : nullptr;
  }

  template <typename T, typename... Ts>
  bool has(Entity e) const
  {
    return has(e, Info::template index<T>()) && (has(e, Info::template index<Ts>()) && ...);
  }

  template <typename... Args>
  requires std::constructible_from<EntityData, Args...>
  [[nodiscard]] Entity create(Args&&... args)
  {
    auto handle = entities_.emplace(std::forward<Args>(args)...);
    return handle.template with_tag<>();
  }

  [[nodiscard]] constexpr u32 capacity() const { return entities_.capacity(); }

  // Gets the number of live entities;
  [[nodiscard]] u32 size() const { return entities_.size(); }

  [[nodiscard]] constexpr u32 limit() const { return entities_.limit(); }

  void clear()
  {
    for (auto& ptr : stores_) ptr.reset();
    entities_.clear();
  }

  struct Iterator
  { ///////////////////////////////////////////////////////////////////
    using iterator_concept                     = std::forward_iterator_tag;
    using iterator_category                    = std::forward_iterator_tag;
    using difference_type                      = std::ptrdiff_t;
    using value_type                           = std::pair<Entity, EntityData>;
    using reference                            = std::pair<Entity, EntityData const&>;
    using pointer                              = void;
    Iterator()                                 = default;
    Iterator(Iterator const& other)            = default;
    Iterator& operator=(Iterator const& other) = default;

    reference operator*() const { return {(*slot_).first.template with_tag<>(), (*slot_).second}; }

    Iterator& operator++()
    {
      ++slot_;
      return *this;
    }

    Iterator operator++(int)
    {
      auto temp = *this;
      ++(*this);
      return temp;
    }

    bool operator==(Iterator const&) const = default;

  private:

    friend struct Registry;

    Iterator(SlotMap<EntityData>::const_iterator slot) : slot_(slot) {}

    SlotMap<EntityData>::const_iterator slot_;
  }; //////////////////////////////////////////////////////////////////////////////////

  [[nodiscard]] Iterator begin() const { return Iterator(entities_.begin()); }

  [[nodiscard]] Iterator end() const { return Iterator(entities_.end()); }

  // Explicit copy to prevent unintended implicit copy construction.
  // Won't work if any of the component types are not copy constructible.
  [[nodiscard]] Registry copy() const { return *this; }

private:

  Registry(Registry const& other) : entities_(other.entities_.copy()), stores_(Info::count())
  {
    INVARIANT(entities_.size() == other.entities_.size());
    for (auto i = 0u; i < Info::count(); ++i) {
      if (other.stores_[i]) stores_[i] = other.stores_[i]->clone();
    }
  }

  static constexpr u32 index(Entity e) noexcept { return e.to_handle().index; }

  static constexpr Handle<EntityData> handle(Entity e) noexcept
  {
    return e.to_handle<EntityData>();
  }

  bool has(Entity e, u32 i) const
  {
    INVARIANT(i < stores_.size(), "component index must be valid");
    return stores_[i] ? stores_[i]->has(index(e)) : false;
  }

  template <typename T>
  [[nodiscard]] constexpr ComponentStore<T>* get_store()
  {
    INVARIANT(
      Info::template index<T>() < stores_.size(),
      "component index must smaller than component count"
    );
    auto& store = stores_[Info::template index<T>()];
    return static_cast<ComponentStore<T>*>(store.get());
  }

  template <typename T>
  [[nodiscard]] constexpr ComponentStore<T>* get_store() const
  {
    INVARIANT(
      Info::template index<T>() < stores_.size(),
      "component index must smaller than component count"
    );
    auto& store = stores_[Info::template index<T>()];
    return static_cast<ComponentStore<T>*>(store.get());
  }

  template <typename T>
  [[nodiscard]] ComponentStore<T>& get_or_create_store()
  {
    INVARIANT(
      Info::template index<T>() < stores_.size(),
      "component index must smaller than component count"
    );
    auto& store = stores_[Info::template index<T>()];
    if (!store) store = std::make_unique<ComponentStore<T>>();
    return *static_cast<ComponentStore<T>*>(store.get());
  }

  SlotMap<EntityData> entities_;
  // vector not array to allow use of a component set that is determined at
  // run-time
  std::vector<std::unique_ptr<AnyComponentStore>> stores_;
}; //////////////////////////////////////////////////////////////////////////////////////

#include "engine/registry.hh"

#include "engine/entity.hh"

#include <set>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace { //////////////////////////////////////////////////////////////////////////////

  struct Position
  {
    float x = 0.f, y = 0.f;
  };
  struct Velocity
  {
    float dx = 0.f, dy = 0.f;
  };
  struct Health
  {
    int hp = 100;
  };

  struct Counted
  {
    static inline int instances = 0;
    int               value;
    explicit Counted(int v = 0)
      : value{v}
    {
      ++instances;
    }
    Counted(const Counted& o)
      : value{o.value}
    {
      ++instances;
    }
    Counted& operator=(const Counted& o) = default;
    ~Counted() { --instances; }
  };

  struct Meta
  {
    std::string name;
  };

  using Components = TypeList<Position, Velocity, Health, Counted>;

} // namespace

TEST_CASE("Registry – entity creation and destruction", "[registry][entity]")
{

  Registry<Components, Meta> r;

  SECTION("creating entities")
  {
    std::set<Entity> entities;
    for (u32 i = 0; i < 10; ++i) {
      auto [e, is_unique] = entities.emplace(r.create());
      REQUIRE(is_unique);
      REQUIRE_FALSE(*e == nil_entity);
      REQUIRE(r.size() == i + 1u);
      REQUIRE(r.is_alive(*e));
    }
  }

  SECTION("destroying entities")
  {
    std::array<Entity, 10> entities;
    for (auto& e : entities) e = r.create();

    for (u32 i = 0; i < 10; ++i) {
      REQUIRE(r.size() == 10u - i);
      auto e = entities[i];
      r.destroy(e);
      REQUIRE_FALSE(r.is_alive(e));
    }
    REQUIRE(r.size() == 0u);
  }

  SECTION("polling non-existent entity")
  {
    Entity e = GENERATE(Handle<>{}, Handle<>{48u, 29u});
    REQUIRE_FALSE(r.is_alive(e));
    REQUIRE_FALSE(r.has<Position>(e));
    REQUIRE(r.get<Position>(e) == nullptr);
  }

  SECTION("polling non-existent component")
  {
    Entity e = r.create();
    GENERATE(1, 2, 3);
    REQUIRE_FALSE(r.has<Position>(e));
    REQUIRE(r.get<Position>(e) == nullptr);
  }

  SECTION("emplacing component")
  {
    Entity   e   = r.create();
    Counted& ref = r.emplace<Counted>(e, 381);
    REQUIRE(Counted::instances == 1u);
    REQUIRE(r.has<Counted>(e));
    REQUIRE(ref.value == 381);
    Counted* ptr = r.get<Counted>(e);
    REQUIRE_FALSE(ptr == nullptr);
    REQUIRE(ptr == &ref);
  }

  SECTION("erasing component")
  {
    Entity e = r.create();
    r.emplace<Counted>(e);
    r.emplace<Health>(e);
    REQUIRE(r.has<Counted>(e));
    REQUIRE(Counted::instances == 1);

    r.erase<Counted>(e);
    REQUIRE_FALSE(r.has<Counted>(e));
    REQUIRE(r.get<Counted>(e) == nullptr);
    // destructor should have been called
    REQUIRE(Counted::instances == 0u);
    // erasing one component should not affect other components
    REQUIRE(r.has<Health>(e));
  }

  SECTION("components should be tied to entity lifetime")
  {
    Entity e = r.create();
    r.emplace<Position>(e);
    r.emplace<Velocity>(e);
    r.emplace<Health>(e);
    r.destroy(e);
    REQUIRE_FALSE(r.has<Position>(e));
    REQUIRE_FALSE(r.has<Velocity>(e));
    REQUIRE_FALSE(r.has<Health>(e));
    REQUIRE(r.get<Position>(e) == nullptr);
    REQUIRE(r.get<Velocity>(e) == nullptr);
    REQUIRE(r.get<Health>(e) == nullptr);
  }

  SECTION("modifying component")
  {
    Entity e = r.create();
    r.emplace<Position>(e, 0.f, 0.f);
    r.get<Position>(e)->x = 99.f;
    REQUIRE(r.get<Position>(e)->x == 99.f);
  }

  SECTION("emplace after erase")
  {
    Entity e = r.create();
    r.emplace<Counted>(e);
    r.erase<Counted>(e);
    r.emplace<Counted>(e, 739);
    REQUIRE(r.get<Counted>(e)->value == 739);
  }

  SECTION("variadic has")
  {
    Entity e = r.create();
    r.emplace<Position>(e);
    REQUIRE_FALSE(r.has<Position, Velocity>(e));
    r.emplace<Velocity>(e);
    REQUIRE(r.has<Position, Velocity>(e));
    r.erase<Velocity>(e);
    REQUIRE_FALSE((r.has<Position, Velocity>(e)));
  }

  SECTION("iterating")
  {
    // iterating empty registry
    int count = 0;
    for (auto [e, _] : r) ++count;
    REQUIRE(count == 0);

    Entity a = r.create(), b = r.create(), c = r.create(), d = r.create();
    bool   saw_a = false, saw_b = false, saw_c = false, saw_d = false;
    r.destroy(d);

    for (auto [e, _] : r) {
      ++count;
      if (e == a) saw_a = true;
      if (e == b) saw_b = true;
      if (e == c) saw_c = true;
      if (e == d) saw_d = true;
    }

    REQUIRE(count == 3);
    REQUIRE(saw_a);
    REQUIRE(saw_b);
    REQUIRE(saw_c);
    // destroyed entity should be absent from iteration
    REQUIRE_FALSE(saw_d);
  }

  SECTION("clearing")
  {
    r.emplace<Counted>(r.create());
    r.emplace<Counted>(r.create());
    REQUIRE(Counted::instances == 2u);
    REQUIRE(r.size() == 2u);

    r.clear();
    REQUIRE(r.size() == 0u);
    REQUIRE(Counted::instances == 0u);

    // registry is usable after clear
    r.emplace<Counted>(r.create());
    REQUIRE(Counted::instances == 1u);
    REQUIRE(r.size() == 1u);
  }

  SECTION("setting and polling per entity meta-data")
  {
    Entity e = r.create("Hello!");
    for (auto [_, meta] : r) REQUIRE(meta.name == "Hello!");
    // meta-data is independent, per-entity
    (void)r.create("George");
    Meta* meta = r.meta(e);
    REQUIRE_FALSE(meta == nullptr);
    REQUIRE(meta->name == "Hello!");
  }
}

